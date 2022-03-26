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
        ~Control();

        void set_rect_position(float x, float y);

        Vec2<float> get_rect_position() const;

        void set_rect_size(float w, float h);

        Vec2<float> get_set_rect_size() const;

        void set_rect_scale(float x, float y);

        Vec2<float> get_rect_scale() const;

        void set_rect_rotation(float r);

        float get_rect_rotation() const;

        void set_rect_pivot_offset(float x, float y);

        Vec2<float> get_rect_pivot_offset() const;

        const std::vector<Vertex> vertices = {
                {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
                {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
                {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
                {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        };

        // For index buffer. (Front is counter-clockwise.)
        const std::vector<uint32_t> indices = {
                0, 2, 1, 2, 0, 3
        };

    protected:
        Vec2<float> rect_position{0};
        Vec2<float> rect_size{128};
        Vec2<float> rect_scale{1};
        float rect_rotation = 0;
        Vec2<float> rect_pivot_offset{0};

        void update(double delta) override;

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
        VkDescriptorPool descriptor_pool;

        /// Descriptor sets are allocated from descriptor pool objects.
        std::vector<VkDescriptorSet> descriptor_sets;

        bool vk_resources_allocated = false;

        void create_vertex_buffer();

        void create_index_buffer();

        void create_uniform_buffers();

        void update_uniform_buffer();
    };
}

#endif //FLINT_CONTROL_H
