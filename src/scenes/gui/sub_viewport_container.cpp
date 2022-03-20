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

    void SubViewportContainer::draw() {
        auto commandBuffer = SwapChain::getSingleton().commandBuffers[SwapChain::getSingleton().currentImage];

        // Pause command recording for the main viewport, and start recording commands for the sub viewport.
        vkCmdEndRenderPass(commandBuffer);

        // Don't call Control::draw(), so we can break the recursive calling
        // to call the sub-viewport draw function below specifically.

        // Start sub-viewport render pass.
        if (viewport != nullptr) {
            viewport->draw();
        }

        // FIXME: We can only begin the main pass once, otherwise it will clear things drawn previously.
        // Bind the main viewport render pass again.
        // Begin render pass.
        // ----------------------------------------------
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = SwapChain::getSingleton().renderPass;
        renderPassInfo.framebuffer = SwapChain::getSingleton().swapChainFramebuffers[SwapChain::getSingleton().currentImage]; // Set target framebuffer.
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = SwapChain::getSingleton().swapChainExtent;

        // Clear color.
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer,
                             &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
        // ----------------------------------------------

        // Now draw the sub-viewport image.
        self_draw();

        Logger::verbose("DRAW", "SubViewportContainer");
    }

    // This will be called by the scene tree.
    void SubViewportContainer::self_draw() {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RS::getSingleton().blitGraphicsPipeline;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->blitGraphicsPipeline;
        }

        VkBuffer vertexBuffers[] = {vertex_buffer};
        RS::getSingleton().blit(
                SwapChain::getSingleton().commandBuffers[SwapChain::getSingleton().currentImage],
                pipeline,
                descriptor_sets[SwapChain::getSingleton().currentImage],
                vertexBuffers,
                index_buffer,
                indices.size());

        Logger::verbose("DRAW", "SubViewportContainer");
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
