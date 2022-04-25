#include "surface.h"

#include "../render/platform.h"
#include "../render/swap_chain.h"
#include "../render/mvp_buffer.h"
#include "default_resource.h"

Surface::~Surface() {
    auto device = Platform::getSingleton().device;

    // Clean up index buffer.
    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);

    // Clean up vertex buffer.
    vkDestroyBuffer(device, vertexBuffer, nullptr); // GPU memory
    vkFreeMemory(device, vertexBufferMemory, nullptr); // CPU memory
}

void Surface2d::create_vertex_buffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer; // In GPU
    VkDeviceMemory stagingBufferMemory; // In CPU

    // Create the GPU buffer and link it with the CPU memory.
    RenderServer::getSingleton().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    stagingBuffer,
                                    stagingBufferMemory);

    // Copy data to the CPU memory.
    RenderServer::getSingleton().copyDataToMemory((void *) vertices.data(), stagingBufferMemory, bufferSize);

    // Create the vertex buffer (GPU) and bind it to the vertex memory (CPU).
    RenderServer::getSingleton().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                    vertexBuffer,
                                    vertexBufferMemory);

    // Copy buffer (GPU).
    RenderServer::getSingleton().copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    // Clean up staging buffer and memory.
    vkDestroyBuffer(Platform::getSingleton().device, stagingBuffer, nullptr);
    vkFreeMemory(Platform::getSingleton().device, stagingBufferMemory, nullptr);
}

void Surface2d::create_index_buffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    // Set indices count for surface.
    indices_count = indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    RenderServer::getSingleton().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    stagingBuffer,
                                    stagingBufferMemory);

    RenderServer::getSingleton().copyDataToMemory((void *) indices.data(),
                                        stagingBufferMemory,
                                        bufferSize);

    RenderServer::getSingleton().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                    indexBuffer,
                                    indexBufferMemory);

    // Copy data from staging buffer to index buffer.
    RenderServer::getSingleton().copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(Platform::getSingleton().device, stagingBuffer, nullptr);
    vkFreeMemory(Platform::getSingleton().device, stagingBufferMemory, nullptr);
}

std::shared_ptr<Material2d> Surface2d::get_material() const {
    if (material == nullptr) {
        return DefaultResource::get_singleton().default_material_2d;
    }

    return material;
}

void Surface2d::set_material(const std::shared_ptr<Material2d>& p_material) {
    material = p_material;
}
