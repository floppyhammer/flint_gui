#include "surface.h"

#include "default_resource.h"

std::shared_ptr<Material2d> Surface2d::get_material() const {
    return material;
}

void Surface2d::set_material(const std::shared_ptr<Material2d> &p_material) {
    material = p_material;
}

std::shared_ptr<Surface2d> Surface2d::from_default() {
    auto surface = std::make_shared<Surface2d>();

    return surface;
}

void Surface3d::set_material(const std::shared_ptr<Material3d> &p_material) {
    material = p_material;
}

std::shared_ptr<Material3d> Surface3d::get_material() const {
    return material;
}

void SurfaceGpuResources::create_vertex_buffer(const std::vector<Vertex> &vertices) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer; // In GPU
    VkDeviceMemory stagingBufferMemory; // In CPU

    // Create the GPU buffer and link it with the CPU memory.
    RenderServer::getSingleton().createBuffer(bufferSize,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
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

void SurfaceGpuResources::create_index_buffer(const std::vector<uint32_t> &indices) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    // Set indices count for surface.
    indices_count = indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    RenderServer::getSingleton().createBuffer(bufferSize,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
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

SurfaceGpuResources::SurfaceGpuResources(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) {
    create_vertex_buffer(vertices);
    create_index_buffer(indices);
}

SurfaceGpuResources::~SurfaceGpuResources() {
    auto device = Platform::getSingleton().device;

    // Clean up index buffer.
    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);

    // Clean up vertex buffer.
    vkDestroyBuffer(device, vertexBuffer, nullptr); // GPU memory
    vkFreeMemory(device, vertexBufferMemory, nullptr); // CPU memory
};

VkBuffer Surface::get_vertex_buffer() {
    if (gpu_resources == nullptr)
        return nullptr;

    return gpu_resources->vertexBuffer;
}

VkBuffer Surface::get_index_buffer() {
    if (gpu_resources == nullptr)
        return nullptr;

    return gpu_resources->indexBuffer;
}

uint32_t Surface::get_index_count() {
    if (gpu_resources == nullptr)
        return 0;

    return gpu_resources->indices_count;
}

void Surface::set_gpu_resources(std::shared_ptr<SurfaceGpuResources> p_gpu_resources) {
    gpu_resources = p_gpu_resources;
}
