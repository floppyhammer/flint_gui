#ifndef FLINT_CONTROL_H
#define FLINT_CONTROL_H

#include <vector>

#include "../node.h"
#include "../../common/vec2.h"
#include "../../rendering/mesh.h"
#include "../../rendering/rendering_server.h"

namespace Flint {
    class Control : public Node {
    public:
        Vec2<float> rect_position = Vec2<float>(0);

        Vec2<float> rect_size = Vec2<float>(128);

        Vec2<float> rect_scale = Vec2<float>(1);

        float rect_rotation = 0;

        Vec2<float> rect_pivot_offset = Vec2<float>(0);

        void set_rect_position(float x, float y);

        void set_rect_size(float w, float h);

        void set_rect_scale(float x, float y);

        void set_rect_rotation(float r);

        void set_rect_pivot_offset(float x, float y);

        const std::vector<Vertex> vertices = {
                {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
                {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
                {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
                {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        };

        // For index buffer.
        const std::vector<uint32_t> indices = {
                0, 1, 2, 2, 3, 0
        };

    protected:
        void update(double delta);

        /// Vertex buffer.
        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;

        /// Index buffer.
        VkBuffer index_buffer;
        VkDeviceMemory index_buffer_memory;

        /// We have a uniform buffer per swap chain image.
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;

        /// A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
        VkDescriptorPool descriptorPool;

        /// Descriptor sets are allocated from descriptor pool objects.
        std::vector<VkDescriptorSet> descriptor_sets;

        bool vkResourcesAllocated = false;

        void create_vertex_buffer();

        void create_index_buffer();

        void update_uniform_buffer();

        void create_uniform_buffers();
    };
}

#endif //FLINT_CONTROL_H
