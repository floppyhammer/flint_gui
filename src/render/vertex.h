#ifndef FLINT_VERTEX_H
#define FLINT_VERTEX_H

#include "../servers/render_server.h"

namespace Flint {

/// Vertex for 2D and 3D meshes.
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;

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

        attributeDescriptions[0] = {0, 0, VK_FORMAT_R32G32B32_SFLOAT, (uint32_t)offsetof(Vertex, pos)};

        attributeDescriptions[1] = {1, 0, VK_FORMAT_R32G32B32_SFLOAT, (uint32_t)offsetof(Vertex, color)};

        attributeDescriptions[2] = {2, 0, VK_FORMAT_R32G32_SFLOAT, (uint32_t)offsetof(Vertex, uv)};

        return attributeDescriptions;
    }
};

/// Vertex for Skeleton2D.
struct SkeletonVertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;
    glm::vec4 bone_indices; // Can be affected by at most 4 bones.
    glm::vec4 bone_weights; // These 4 bones' weights.

    /// Vertex binding description.
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription = {0, sizeof(SkeletonVertex), VK_VERTEX_INPUT_RATE_VERTEX};

        return bindingDescription;
    }

    /// Vertex attribute descriptions.
    static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

        attributeDescriptions[0] = {0, 0, VK_FORMAT_R32G32B32_SFLOAT, (uint32_t)offsetof(SkeletonVertex, pos)};
        attributeDescriptions[1] = {1, 0, VK_FORMAT_R32G32B32_SFLOAT, (uint32_t)offsetof(SkeletonVertex, color)};
        attributeDescriptions[2] = {2, 0, VK_FORMAT_R32G32_SFLOAT, (uint32_t)offsetof(SkeletonVertex, uv)};
        attributeDescriptions[3] = {
            3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, (uint32_t)offsetof(SkeletonVertex, bone_indices)};
        attributeDescriptions[4] = {
            4, 0, VK_FORMAT_R32G32B32A32_SFLOAT, (uint32_t)offsetof(SkeletonVertex, bone_weights)};

        return attributeDescriptions;
    }
};

/// Vertex for Skybox.
struct SkyboxVertex {
    glm::vec3 pos;

    /// Vertex binding description.
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription = {0, sizeof(SkyboxVertex), VK_VERTEX_INPUT_RATE_VERTEX};

        return bindingDescription;
    }

    /// Vertex attribute descriptions.
    static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

        attributeDescriptions[0] = {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0};

        return attributeDescriptions;
    }
};

} // namespace Flint

#endif // FLINT_VERTEX_H
