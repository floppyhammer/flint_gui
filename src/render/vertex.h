#ifndef FLINT_VERTEX_H
#define FLINT_VERTEX_H

#include "render_server.h"

namespace Flint {
/// Vertex for 2D and 3D meshes.
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv; // Texture coordinates.

    /// For hashmap.
    bool operator==(const Vertex &other) const {
        return pos == other.pos && color == other.color && uv == other.uv;
    }

    /// Vertex binding description.
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate =
            VK_VERTEX_INPUT_RATE_VERTEX; // Specify rate at which vertex attributes are pulled from buffers.

        return bindingDescription;
    }

    /// Vertex attribute descriptions.
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, uv);

        return attributeDescriptions;
    }
};

/// Vertex for Skeleton2D.
struct SkeletonVertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;           // Texture coordinates.
    glm::vec4 bone_indices; // Can be affected by at most 4 bones.
    glm::vec4 bone_weights; // These 4 bones' weights.

    /// Vertex binding description.
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(SkeletonVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    /// Vertex attribute descriptions.
    static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(SkeletonVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(SkeletonVertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(SkeletonVertex, uv);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(SkeletonVertex, bone_indices);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(SkeletonVertex, bone_weights);

        return attributeDescriptions;
    }
};

/// Vertex for Skybox.
struct SkyboxVertex {
    glm::vec3 pos;

    /// Vertex binding description.
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(SkyboxVertex);
        bindingDescription.inputRate =
            VK_VERTEX_INPUT_RATE_VERTEX; // Specify rate at which vertex attributes are pulled from buffers.

        return bindingDescription;
    }

    /// Vertex attribute descriptions.
    static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = 0;

        return attributeDescriptions;
    }
};

/// GPU data containing a vertex and index buffer.
template <typename T>
struct VertexGpuResources {
    VertexGpuResources(const std::vector<T> &vertices, const std::vector<uint32_t> &indices) {
        create_vertex_buffer(vertices);
        create_index_buffer(indices);
    }

    ~VertexGpuResources() {
        auto device = Platform::getSingleton()->device;

        // Clean up index buffer.
        vkDestroyBuffer(device, index_buffer, nullptr);
        vkFreeMemory(device, index_buffer_memory, nullptr);

        // Clean up vertex buffer.
        vkDestroyBuffer(device, vertex_buffer, nullptr);
        vkFreeMemory(device, vertex_buffer_memory, nullptr);
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

    /// Index buffer.
    VkBuffer index_buffer{};
    VkDeviceMemory index_buffer_memory{};
    uint32_t index_count = 0;

private:
    void create_vertex_buffer(const std::vector<T> &vertices) {
        VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

        VkBuffer staging_buffer;              // In GPU
        VkDeviceMemory staging_buffer_memory; // In CPU

        auto rs = RenderServer::getSingleton();

        // Create the GPU buffer and link it with the GPU memory.
        rs->createBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         staging_buffer,
                         staging_buffer_memory);

        // Copy data to the device memory.
        rs->copyDataToMemory((void *)vertices.data(), staging_buffer_memory, buffer_size);

        // Create the vertex buffer (GPU) and bind it to the vertex memory (CPU).
        rs->createBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         vertex_buffer,
                         vertex_buffer_memory);

        // Copy buffer (GPU).
        rs->copyBuffer(staging_buffer, vertex_buffer, buffer_size);

        // Clean up staging buffer and memory.
        vkDestroyBuffer(Platform::getSingleton()->device, staging_buffer, nullptr);
        vkFreeMemory(Platform::getSingleton()->device, staging_buffer_memory, nullptr);
    }

    void create_index_buffer(const std::vector<uint32_t> &indices) {
        VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();

        // Set indices count for surface.
        index_count = indices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        auto rs = RenderServer::getSingleton();

        rs->createBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         staging_buffer,
                         staging_buffer_memory);

        rs->copyDataToMemory((void *)indices.data(), staging_buffer_memory, buffer_size);

        rs->createBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         index_buffer,
                         index_buffer_memory);

        // Copy data from staging buffer to index buffer.
        rs->copyBuffer(staging_buffer, index_buffer, buffer_size);

        vkDestroyBuffer(Platform::getSingleton()->device, staging_buffer, nullptr);
        vkFreeMemory(Platform::getSingleton()->device, staging_buffer_memory, nullptr);
    }
};
} // namespace Flint

#endif // FLINT_VERTEX_H
