#ifndef FLINT_CONTROL_H
#define FLINT_CONTROL_H

#include <vector>

#include "../node.h"
#include "../../../common/geometry.h"
#include "../../../resources/mesh.h"
#include "../../../render/mvp.h"
#include "../../../render/render_server.h"
#include "../../../servers/input_server.h"
#include "../../../servers/vector_server.h"
#include "../../../resources/style_box.h"

using Pathfinder::ColorF;

namespace Flint {
    /// How a control node handles mouse input propagation.
    enum class MouseFilter {
        STOP, // Use input and mark it as consumed.
        PASS, // Use input.
        IGNORE, // Ignore input.
    };

    /// Control anchor only takes effect when the control node is not a child of a container
    /// but that of a normal control node.
    enum class AnchorFlag {
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_RIGHT,
        BOTTOM_LEFT,

        CENTER_LEFT,
        CENTER_TOP,
        CENTER_RIGHT,
        CENTER_BOTTOM,
        CENTER,

        LEFT_WIDE,
        TOP_WIDE,
        RIGHT_WIDE,
        BOTTOM_WIDE,
        VCENTER_WIDE,
        HCENTER_WIDE,

        FULL_RECT,

        MAX,
    };

    /// How a parent container organizes this control node.
    enum class ContainerSizingFlag {
        Expand,
        Shrink,
    };

    class Control : public Node {
    public:
        Control();

        virtual void set_position(Vec2<float> p_position);

        virtual Vec2<float> get_position() const;

        virtual void set_size(Vec2<float> p_size);

        virtual Vec2<float> get_size() const;

        virtual void set_minimum_size(Vec2<float> p_minimum_size);

        virtual Vec2<float> get_minimum_size() const;

        virtual Vec2<float> calculate_minimum_size() const;

        Vec2<float> get_global_position() const;

        void draw(VkCommandBuffer p_command_buffer) override;

        void set_mouse_filter(MouseFilter filter);

        ContainerSizingFlag sizing_flag = ContainerSizingFlag::Shrink;

        void set_visibility(bool p_visible);

        bool get_visibility() const;

        Vec2F get_local_mouse_position() const;

        virtual void grab_focus();

        virtual void release_focus();

        ColorF get_global_modulate();

        /**
         * Check if the node is a child of a container.
         * @return
         */
        bool is_inside_container() const;

        Vec2F get_max_child_min_size() const;

        ColorF modulate;
        ColorF self_modulate;

        /**
         * Adjust the node's position and size according to the anchor flag.
         */
        void apply_anchor();

        void set_anchor_flag(AnchorFlag flag);
        void get_anchor_flag();

    protected:
        Vec2<float> position{0};
        Vec2<float> size{128};
        Vec2<float> scale{1};
        Vec2<float> pivot_offset{0}; // Top-left as the default pivot.
        float rotation = 0;

        bool focused = false;

        bool is_pressed_inside = false;

        Vec2<float> minimum_size{0};

        Vec2F local_mouse_position;

        AnchorFlag anchor_mode = AnchorFlag::MAX;

        void update(double dt) override;

        void input(std::vector<InputEvent> &input_queue) override;

        bool is_cursor_inside = false;

        virtual void cursor_entered();

        virtual void cursor_exited();

        bool visible = true;

        /// Visualize the node's size.
        StyleBox debug_size_box;

        MouseFilter mouse_filter = MouseFilter::STOP;
    };
}

#endif //FLINT_CONTROL_H
