#include "material.h"

#include "../render/swap_chain.h"

namespace Flint {
    Material2dDescSet::Material2dDescSet() {
        createDescriptorPool();

        createDescriptorSet();
    }

    Material3dDescSet::Material3dDescSet() {
        createDescriptorPool();

        createDescriptorSet();
    }

    VkDescriptorSet MaterialDescSet::getDescriptorSet(uint32_t index) const {
        if (index < descriptorSets.size()) {
            return descriptorSets[index];
        } else {
            throw std::runtime_error("Invalid descriptor set index!");
        }
    }

    MaterialDescSet::~MaterialDescSet() {
        auto device = Platform::getSingleton()->device;

        // When we destroy the pool, the sets inside are destroyed as well.
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }

    void Material3dDescSet::createDescriptorPool() {
        auto swapChainImages = SwapChain::getSingleton()->swapChainImages;

        std::array<VkDescriptorPoolSize, 1> poolSizes{};
//    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

        if (vkCreateDescriptorPool(Platform::getSingleton()->device, &poolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    void Material3dDescSet::createDescriptorSet() {
        auto device = Platform::getSingleton()->device;
        auto swapChainImages = SwapChain::getSingleton()->swapChainImages;
        auto &descriptorSetLayout = RenderServer::getSingleton()->meshDescriptorSetLayout;

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

    void Material3dDescSet::updateDescriptorSet(const std::shared_ptr<ImageTexture> &p_texture) {
        auto swapChainImages = SwapChain::getSingleton()->swapChainImages;
        auto &descriptorSetLayout = RenderServer::getSingleton()->meshDescriptorSetLayout;
        auto device = Platform::getSingleton()->device;

        for (size_t i = 0; i < swapChainImages.size(); i++) {
//        VkDescriptorBufferInfo bufferInfo{};
//        bufferInfo.buffer = uniformBuffers[i];
//        bufferInfo.offset = 0;
//        bufferInfo.range = sizeof(Flint::ModelViewProjection);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = p_texture->imageView;
            imageInfo.sampler = p_texture->sampler;

            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

//        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[0].dstSet = descriptorSets[i];
//        descriptorWrites[0].dstBinding = 0;
//        descriptorWrites[0].dstArrayElement = 0;
//        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//        descriptorWrites[0].descriptorCount = 1;
//        descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pImageInfo = &imageInfo;

            // Update the contents of a descriptor set object.
            vkUpdateDescriptorSets(device,
                                   static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(),
                                   0,
                                   nullptr);
        }
    }

    void Material2dDescSet::createDescriptorPool() {
        auto swapChainImages = SwapChain::getSingleton()->swapChainImages;

        std::array<VkDescriptorPoolSize, 1> poolSizes{};
//    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

        if (vkCreateDescriptorPool(Platform::getSingleton()->device, &poolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    void Material2dDescSet::createDescriptorSet() {
        auto device = Platform::getSingleton()->device;
        auto swapChainImages = SwapChain::getSingleton()->swapChainImages;
        auto &descriptorSetLayout = RenderServer::getSingleton()->blitDescriptorSetLayout;

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

    void Material2dDescSet::updateDescriptorSet(const std::shared_ptr<ImageTexture> &p_texture) {
        auto swapChainImages = SwapChain::getSingleton()->swapChainImages;
        auto &descriptorSetLayout = RenderServer::getSingleton()->blitDescriptorSetLayout;
        auto device = Platform::getSingleton()->device;

        for (size_t i = 0; i < swapChainImages.size(); i++) {
//        VkDescriptorBufferInfo bufferInfo{};
//        bufferInfo.buffer = uniformBuffers[i];
//        bufferInfo.offset = 0;
//        bufferInfo.range = sizeof(glm::mat4);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = p_texture->imageView;
            imageInfo.sampler = p_texture->sampler;

            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

//        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[0].dstSet = descriptorSets[i];
//        descriptorWrites[0].dstBinding = 0;
//        descriptorWrites[0].dstArrayElement = 0;
//        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//        descriptorWrites[0].descriptorCount = 1;
//        descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pImageInfo = &imageInfo;

            // Update the contents of a descriptor set object.
            vkUpdateDescriptorSets(device,
                                   static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(),
                                   0,
                                   nullptr);
        }
    }

    Material2d::Material2d() {
        desc_set = std::make_shared<Material2dDescSet>();
    }

    void Material2d::set_texture(const std::shared_ptr<ImageTexture> &p_texture) {
        texture = p_texture;

        desc_set->updateDescriptorSet(p_texture);
    }

    std::shared_ptr<ImageTexture> Material2d::get_texture() {
        return texture;
    }

    Material3d::Material3d() {
        desc_set = std::make_shared<Material3dDescSet>();
    }

    std::shared_ptr<Material3d> Material3d::from_default() {
        auto material = std::make_shared<Material3d>();
        material->diffuse_texture = ImageTexture::from_empty(4, 4);

        return material;
    }

    void Material3d::set_diffuse_texture(std::shared_ptr<ImageTexture> p_texture) {
        diffuse_texture = p_texture;

        desc_set->updateDescriptorSet(p_texture);
    }
}
