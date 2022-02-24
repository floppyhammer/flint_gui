#ifndef FLINT_CONTROL_H
#define FLINT_CONTROL_H

#include <vector>

#include "../node.h"
#include "../../common/vec2.h"
#include "../../rendering/rendering_server.h"

namespace Flint {
    struct Mvp2d {
        glm::mat3 model;
        glm::mat3 view;
        glm::mat3 proj;
    };

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

    protected:
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
    };
}

#endif //FLINT_CONTROL_H
