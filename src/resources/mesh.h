#ifndef FLINT_RENDER_MESH_H
#define FLINT_RENDER_MESH_H

#include "../servers/render_server.h"
#include "../servers/display_server.h"

namespace Flint {

/// CPU data containing vertex and index data.
template <typename T>
struct MeshCpu {
    std::vector<T> vertices;
    std::vector<uint32_t> indices;

    MeshCpu() = default;

    MeshCpu(const std::vector<T> &_vertices, const std::vector<uint32_t> &_indices)
        : vertices(_vertices), indices(_indices) {
    }
};

/// GPU data containing vertex and index data.
template <typename T>
struct MeshGpu {
    MeshGpu() = default;

    MeshGpu(std::vector<T> vertices, std::vector<uint32_t> indices) {
        upload_vertices(vertices);
        upload_indices(indices);
    }

    ~MeshGpu() {
        free_vertices();
        free_indices();
    }

    void upload(const std::shared_ptr<MeshCpu<T>> &mesh) {
        upload_vertices(mesh->vertices);
        upload_indices(mesh->indices);
    }

    void free_vertices() {
        if (!vertex_buffer && !vertex_buffer_memory) {
            return;
        }

        auto device = DisplayServer::get_singleton()->get_device();

        // Clean up index buffer.
        vkDestroyBuffer(device, vertex_buffer, nullptr);
        vkFreeMemory(device, vertex_buffer_memory, nullptr);

        vertex_buffer = nullptr;
        vertex_buffer_memory = nullptr;
    }

    void free_indices() {
        if (!index_buffer && !index_buffer_memory) {
            return;
        }

        auto device = DisplayServer::get_singleton()->get_device();

        // Clean up index buffer.
        vkDestroyBuffer(device, index_buffer, nullptr);
        vkFreeMemory(device, index_buffer_memory, nullptr);

        index_buffer = nullptr;
        index_buffer_memory = nullptr;
    }

    VkBuffer get_vertex_buffer() {
        return vertex_buffer;
    }

    VkBuffer get_index_buffer() {
        return index_buffer;
    }

    uint32_t get_index_count() {
        return index_count;
    }

private:
    /// Vertex buffer.
    VkBuffer vertex_buffer{};
    VkDeviceMemory vertex_buffer_memory{};
    uint32_t vertex_capacity{};

    /// Index buffer.
    VkBuffer index_buffer{};
    VkDeviceMemory index_buffer_memory{};
    uint32_t index_capacity{};
    uint32_t index_count{};

private:
    void upload_vertices(const std::vector<T> &vertices) {
        if (vertices.empty()) {
            return;
        }

        auto rs = RenderServer::get_singleton();
        auto device = DisplayServer::get_singleton()->get_device();

        VkDeviceSize buffer_size = sizeof(T) * vertices.size();

        // Recreate buffer.
        if (vertex_capacity < vertices.size()) {
            free_vertices();

            // Create the vertex buffer (GPU) and bind it to the vertex memory (CPU).
            rs->createBuffer(buffer_size,
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             vertex_buffer,
                             vertex_buffer_memory);

            vertex_capacity = vertices.size();
        }

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        // Create the GPU buffer and link it with the GPU memory.
        rs->createBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         staging_buffer,
                         staging_buffer_memory);

        // Copy data to the device memory.
        rs->copyDataToMemory((void *)vertices.data(), staging_buffer_memory, buffer_size);

        // Copy buffer.
        rs->copyBuffer(staging_buffer, vertex_buffer, buffer_size);

        // Clean up staging buffer and memory.
        vkDestroyBuffer(device, staging_buffer, nullptr);
        vkFreeMemory(device, staging_buffer_memory, nullptr);
    }

    void upload_indices(const std::vector<uint32_t> &indices) {
        if (indices.empty()) {
            return;
        }

        auto rs = RenderServer::get_singleton();
        auto device = DisplayServer::get_singleton()->get_device();

        VkDeviceSize buffer_size = sizeof(uint32_t) * indices.size();

        if (index_capacity < indices.size()) {
            free_indices();

            rs->createBuffer(buffer_size,
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             index_buffer,
                             index_buffer_memory);

            index_capacity = indices.size();
        }

        // Set indices count for surface.
        index_count = indices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        rs->createBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         staging_buffer,
                         staging_buffer_memory);

        rs->copyDataToMemory((void *)indices.data(), staging_buffer_memory, buffer_size);

        // Copy data from staging buffer to index buffer.
        rs->copyBuffer(staging_buffer, index_buffer, buffer_size);

        vkDestroyBuffer(device, staging_buffer, nullptr);
        vkFreeMemory(device, staging_buffer_memory, nullptr);
    }
};

} // namespace Flint

#endif // FLINT_RENDER_MESH_H
