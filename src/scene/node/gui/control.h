#ifndef FLINT_CONTROL_H
#define FLINT_CONTROL_H

#include <vector>

#include "../node.h"
#include "../../../common/vec2.h"
#include "../../../resources/mesh.h"
#include "../../../render/mvp_buffer.h"
#include "../../../render/render_server.h"
#include "../../../servers/input_server.h"
#include "../../../servers/vector_server.h"
#include "../../../resources/style_box.h"

namespace Flint {
    /// How should this control node handle mouse input propagation.
    enum class MouseFilter {
        STOP,
        PASS,
        IGNORE,
    };

    class Control : public Node {
    public:
        Control();

        ~Control() = default;

        virtual void set_position(Vec2<float> p_position);

        virtual Vec2<float> get_position() const;

        virtual void set_size(Vec2<float> p_size);

        virtual Vec2<float> get_size() const;

        /// Used when being a child of a container.
        Vec2<float> minimum_size{0};

        virtual Vec2<float> calculate_minimum_size();

        Vec2<float> get_global_position() const;

        void draw(VkCommandBuffer p_command_buffer) override;

        void set_mouse_filter(MouseFilter filter);

    protected:
        Vec2<float> position{0};
        Vec2<float> size{128};
        Vec2<float> scale{1};
        Vec2<float> pivot_offset{0}; // Top-left as the default pivot.
        float rotation = 0;

        void update(double dt) override;

        void update_mvp();

        void input(std::vector<InputEvent> &input_queue) override;

        StyleBox outline;

        Surface2dPushConstant push_constant;

        MouseFilter mouse_filter = MouseFilter::STOP;
    };
}

#endif //FLINT_CONTROL_H
