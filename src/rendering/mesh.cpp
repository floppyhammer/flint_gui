#include "mesh.h"

#include "device.h"
#include "swap_chain.h"
#include "mvp_buffer.h"

Mesh3D::Mesh3D() {
    createDescriptorPool();

    createDescriptorSets();
}

Mesh2D::Mesh2D() {
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

VkDescriptorSet Mesh::getDescriptorSet(uint32_t index) const {
    if (index < descriptorSets.size()) {
        return descriptorSets[index];
    } else {
        throw std::runtime_error("Invalid descriptor set index!");
    }
}

void Mesh3D::createDescriptorPool() {
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

void Mesh3D::createDescriptorSets() {
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

void Mesh3D::updateDescriptorSets(std::shared_ptr<Material> p_material, std::vector<VkBuffer> &uniformBuffers) {
    // Cast to Material3D.
    auto material = std::static_pointer_cast<Material3D>(p_material);

    auto swapChainImages = SwapChain::getSingleton().swapChainImages;
    auto &descriptorSetLayout = RS::getSingleton().meshDescriptorSetLayout;
    auto device = Device::getSingleton().device;

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(Flint::UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = material->diffuse_texture->imageView;
        imageInfo.sampler = material->diffuse_texture->sampler;

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

// Mesh2D

void Mesh2D::createDescriptorPool() {
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

void Mesh2D::createDescriptorSets() {
    auto device = Device::getSingleton().device;
    auto swapChainImages = SwapChain::getSingleton().swapChainImages;
    auto &descriptorSetLayout = RS::getSingleton().blitDescriptorSetLayout;

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

void Mesh2D::create_vertex_buffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer; // In GPU
    VkDeviceMemory stagingBufferMemory; // In CPU

    // Create the GPU buffer and link it with the CPU memory.
    RS::getSingleton().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    stagingBuffer,
                                    stagingBufferMemory);

    // Copy data to the CPU memory.
    RS::getSingleton().copyDataToMemory((void *) vertices.data(), stagingBufferMemory, bufferSize);

    // Create the vertex buffer (GPU) and bind it to the vertex memory (CPU).
    RS::getSingleton().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                    vertexBuffer,
                                    vertexBufferMemory);

    // Copy buffer (GPU).
    RS::getSingleton().copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    // Clean up staging buffer and memory.
    vkDestroyBuffer(Device::getSingleton().device, stagingBuffer, nullptr);
    vkFreeMemory(Device::getSingleton().device, stagingBufferMemory, nullptr);
}

void Mesh2D::create_index_buffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    // Set indices count for mesh.
    indices_count = indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    RS::getSingleton().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    stagingBuffer,
                                    stagingBufferMemory);

    RS::getSingleton().copyDataToMemory((void *) indices.data(),
                                        stagingBufferMemory,
                                        bufferSize);

    RS::getSingleton().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                    indexBuffer,
                                    indexBufferMemory);

    // Copy data from staging buffer to index buffer.
    RS::getSingleton().copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(Device::getSingleton().device, stagingBuffer, nullptr);
    vkFreeMemory(Device::getSingleton().device, stagingBufferMemory, nullptr);
}

void Mesh2D::updateDescriptorSets(std::shared_ptr<Material> p_material, std::vector<VkBuffer> &uniformBuffers) {
    // Cast to Material3D.
    auto material = std::static_pointer_cast<Material2D>(p_material);

    auto swapChainImages = SwapChain::getSingleton().swapChainImages;
    auto &descriptorSetLayout = RS::getSingleton().blitDescriptorSetLayout;
    auto device = Device::getSingleton().device;

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(glm::mat4);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = material->texture->imageView;
        imageInfo.sampler = material->texture->sampler;

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
