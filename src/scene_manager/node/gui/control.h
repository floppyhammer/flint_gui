#ifndef FLINT_CONTROL_H
#define FLINT_CONTROL_H

#include <vector>

#include "../node.h"
#include "../../../common/vec2.h"
#include "../../../rendering/mesh.h"
#include "../../../rendering/rendering_server.h"
#include "../../../rendering/mvp_uniform_buffer.h"

namespace Flint {
    class Control : public Node {
    public:
        Control();

        ~Control() = default;

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

    protected:
        Vec2<float> rect_position{0};
        Vec2<float> rect_size{128};
        Vec2<float> rect_scale{1};
        float rect_rotation = 0;
        Vec2<float> rect_pivot_offset{0};

        void _update(double delta) override;

        void update(double delta) override;

        std::shared_ptr<MvpBuffer> mvp_buffer;

        void update_mvp();

        std::shared_ptr<Mesh2D> mesh;

        std::shared_ptr<Material2D> material;

        void init_default_mesh();
    };
}

#endif //FLINT_CONTROL_H
