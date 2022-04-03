#ifndef FLINT_COMPONENTS_H
#define FLINT_COMPONENTS_H

#include "../../../common/math/vec2.h"
#include "../../../common/math/vec3.h"
#include "../../../common/math/quaternion.h"
#include "../../../rendering/mesh.h"
#include "../../../rendering/material.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

namespace Flint {
    // PHYSICS
    // -----------------------------
    struct RigidBody {
        Vec3<float> velocity;
        Vec3<float> acceleration;
    };

    struct Transform3D {
        Vec3<float> position;
        Vec3<float> scale;
        Quaternion rotation;
    };

    struct Transform2D {
        Vec2<float> position;
        Vec2<float> scale;
        float rotation;
    };

    struct Gravity {
        Vec3<float> force;
    };
    // -----------------------------

    // RENDER
    // -----------------------------
    struct Sprite2D {
        /// Vertex buffer.
        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;

        /// Index buffer.
        VkBuffer index_buffer;
        VkDeviceMemory index_buffer_memory;
        uint32_t indices_count = 0;

        /// We have a uniform buffer per swap chain image.
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;

        /// A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
        VkDescriptorPool descriptor_pool;

        /// Descriptor sets are allocated from descriptor pool objects.
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct TransformGUI {
        Vec2<float> rect_position{0};
        Vec2<float> rect_size{128};
        Vec2<float> rect_scale{1};
        Vec2<float> rect_pivot_offset{0}; // Top-left as the default origin.
        float rect_rotation = 0;
    };

    struct Sprite3D {

    };

    struct Model {
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<Material> materials;

        /// We have a uniform buffer per swap chain image.
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
    };
    // -----------------------------
}

#endif //FLINT_COMPONENTS_H
