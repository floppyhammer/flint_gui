#include "sub_viewport_container.h"

#include "../sub_viewport.h"
#include "../../rendering/swap_chain.h"

#include <utility>
#include <array>

namespace Flint {
    SubViewportContainer::SubViewportContainer() {
        type = NodeType::SubViewportContainer;

        create_descriptor_pool();

        create_descriptor_sets();

        create_uniform_buffers();

        create_vertex_buffer();

        create_index_buffer();

        vk_resources_allocated = true;
    }

    void SubViewportContainer::set_viewport(std::shared_ptr<SubViewport> p_viewport) {
        viewport = std::move(p_viewport);

        update_descriptor_sets();
    }

    void SubViewportContainer::update(double delta) {
        // Branch to root.
        Control::update(delta);
    }

    void SubViewportContainer::draw(VkCommandBuffer p_command_buffer) {
        // Don't call Control::draw(), so we can break the recursive calling
        // to call the sub-viewport draw function below specifically.

        auto sub_viewport_command_buffer = RS::getSingleton().beginSingleTimeCommands();

        // Start sub-viewport render pass.
        if (viewport != nullptr) {
            viewport->draw(sub_viewport_command_buffer);
        }

        RS::getSingleton().endSingleTimeCommands(sub_viewport_command_buffer);

        // Now draw the sub-viewport image.
        self_draw(p_command_buffer);

        Logger::verbose("DRAW", "SubViewportContainer");
    }

    // This will be called by the scene tree.
    void SubViewportContainer::self_draw(VkCommandBuffer p_command_buffer) {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RS::getSingleton().blitGraphicsPipeline;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->blitGraphicsPipeline;
        }

        VkBuffer vertexBuffers[] = {vertex_buffer};
        RS::getSingleton().blit(
                p_command_buffer,
                pipeline,
                descriptor_sets[SwapChain::getSingleton().currentImage],
                vertexBuffers,
                index_buffer,
                indices.size());

        Logger::verbose("SELF_DRAW", "SubViewportContainer");
    }

    // Create descriptor pool before creating descriptor sets.
    void SubViewportContainer::create_descriptor_pool() {
        auto swapChainImages = SwapChain::getSingleton().swapChainImages;

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

        if (vkCreateDescriptorPool(Device::getSingleton().device, &poolInfo, nullptr, &descriptor_pool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    void SubViewportContainer::create_descriptor_sets() {
        auto device = Device::getSingleton().device;
        auto swapChainImages = SwapChain::getSingleton().swapChainImages;
        auto &descriptorSetLayout = RS::getSingleton().blitDescriptorSetLayout;

        std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptor_pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        descriptor_sets.resize(swapChainImages.size());
        if (vkAllocateDescriptorSets(device, &allocInfo, descriptor_sets.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }
    }

    void SubViewportContainer::update_descriptor_sets() {
        auto swapChainImages = SwapChain::getSingleton().swapChainImages;
        auto &descriptorSetLayout = RS::getSingleton().blitDescriptorSetLayout;
        auto device = Device::getSingleton().device;

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniform_buffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(glm::mat4);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = viewport->texture->imageView;
            imageInfo.sampler = viewport->texture->sampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptor_sets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptor_sets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            // Update the contents of a descriptor set object.
            vkUpdateDescriptorSets(device,
                                   static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(),
                                   0,
                                   nullptr);
        }
    }
}
