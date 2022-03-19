#include "sprite_3d.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../core/engine.h"
#include "../../rendering/swap_chain.h"

#include <memory>
#include <chrono>
#include <vector>

namespace Flint {
    const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    // For index buffer.
    const std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0
    };

    Sprite3D::Sprite3D() {
        createDescriptorPool();

        createDescriptorSets();

        createUniformBuffers();

        mesh = std::make_shared<Mesh>(vertices, indices);

        createVertexBuffer();
        createIndexBuffer();
    }

    void Sprite3D::set_texture(std::shared_ptr<Texture> p_texture) {
        texture = std::move(p_texture);

        updateDescriptorSets();
    }

    void Sprite3D::update(double delta) {
        Node3D::update(delta);
    }

    // This will be called by the scene tree.
    void Sprite3D::draw() {
        Node3D::draw();

        if (mesh == nullptr || texture == nullptr) return;

        VkBuffer vertexBuffers[] = {vertexBuffer};
        RS::getSingleton().draw_mesh(SwapChain::getSingleton().commandBuffers[SwapChain::getSingleton().currentImage],
                                     RS::getSingleton().meshGraphicsPipeline,
                                     descriptorSets[SwapChain::getSingleton().currentImage],
                                     vertexBuffers,
                                     indexBuffer,
                                     mesh->indices.size());
    }

    // Create descriptor pool before creating descriptor sets.
    void Sprite3D::createDescriptorPool() {
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

        if (vkCreateDescriptorPool(Device::getSingleton().device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    void Sprite3D::createDescriptorSets() {
        auto device = Device::getSingleton().device;
        auto swapChainImages = SwapChain::getSingleton().swapChainImages;
        auto &descriptorSetLayout = RS::getSingleton().meshDescriptorSetLayout;

        std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(swapChainImages.size());
        if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }
    }

    void Sprite3D::updateDescriptorSets() {
        auto swapChainImages = SwapChain::getSingleton().swapChainImages;
        auto &descriptorSetLayout = RS::getSingleton().meshDescriptorSetLayout;
        auto device = Device::getSingleton().device;

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture->imageView;
            imageInfo.sampler = texture->sampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
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
