#include "mesh.h"

#include "device.h"
#include "swap_chain.h"

Mesh::Mesh() {
    createDescriptorPool();

    createDescriptorSets();
}

Mesh::~Mesh() {
    auto device = Device::getSingleton().device;

    // Clean up index buffer.
    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);

    // Clean up vertex buffer.
    vkDestroyBuffer(device, vertexBuffer, nullptr); // GPU memory
    vkFreeMemory(device, vertexBufferMemory, nullptr); // CPU memory

    // When we destroy the pool, the sets inside are destroyed as well.
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

// Create descriptor pool before creating descriptor sets.
void Mesh::createDescriptorPool() {
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

void Mesh::createDescriptorSets() {
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

void Mesh::updateDescriptorSets(Material &material, std::vector<VkBuffer> &uniformBuffers) {
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
        imageInfo.imageView = material.diffuse_texture->imageView;
        imageInfo.sampler = material.diffuse_texture->sampler;

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

VkDescriptorSet Mesh::getDescriptorSet(uint32_t index) const {
    if (index < descriptorSets.size()) {
        return descriptorSets[index];
    } else {
        throw std::runtime_error("Invalid descriptor set index!");
    }
}
