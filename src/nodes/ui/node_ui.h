#ifndef FLINT_NODE_UI_H
#define FLINT_NODE_UI_H

#include <vector>

#include "../../common/geometry.h"
#include "../../resources/style_box.h"
#include "../../servers/input_server.h"
#include "../../servers/vector_server.h"
#include "../node.h"

using Pathfinder::ColorF;

namespace Flint {

/// How a GUI node handles mouse input propagation.
enum class MouseFilter {
    Stop,   // Use input and mark it as consumed.
    Pass,   // Use input.
    Ignore, // Ignore input.
};

/// Anchor takes effect only when the GUI node is
/// inside a non-container control node.
enum class AnchorFlag {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,

    CenterLeft,
    CenterRight,
    CenterTop,
    CenterBottom,
    Center,

    LeftWide,
    RightWide,
    TopWide,
    BottomWide,
    VCenterWide,
    HCenterWide,

    FullRect,

    Max,
};

/// How a parent container organizes this control node.
enum class ContainerSizingFlag {
    Fill,
    ShrinkStart,
    ShrinkCenter,
    ShrinkEnd,
};

struct ContainerSizing {
    ContainerSizingFlag flag_h = ContainerSizingFlag::Fill;
    bool expand_h = false;
    ContainerSizingFlag flag_v = ContainerSizingFlag::Fill;
    bool expand_v = false;
};

class NodeUi : public Node {
public:
    NodeUi();

    virtual void set_position(Vec2F new_position);

    virtual Vec2F get_position() const;

    virtual void set_size(Vec2F new_size);

    virtual Vec2F get_size() const;

    virtual void set_minimum_size(Vec2F new_minimum_size);

    virtual Vec2F get_minimum_size() const;

    virtual Vec2F calc_minimum_size() const;

    bool is_ui_node() const override {
        return true;
    }

    Vec2F get_global_position() const;

    virtual void draw();

    void propagate_draw() override;

    void set_mouse_filter(MouseFilter filter);

    ContainerSizing container_sizing{};

    Vec2F get_local_mouse_position() const;

    virtual void grab_focus();

    virtual void release_focus();

    ColorU get_global_modulate();

    /**
     * Check if the node is a child of a container.
     * @return
     */
    bool is_inside_container() const;

    Vec2F get_max_child_min_size() const;

    ColorU modulate = ColorU::white();
    ColorU self_modulate = ColorU::white();

    /**
     * Adjust the node's position and size according to the anchor flag.
     */
    void apply_anchor();

    void set_anchor_flag(AnchorFlag new_flag);

    void get_anchor_flag();

    void when_window_size_changed(Vec2I new_size);

    void when_cursor_entered();

    void when_cursor_exited();

protected:
    Vec2F position{0};
    Vec2F size{128};
    Vec2F scale{1};
    Vec2F pivot_offset{0}; // Top-left as the default pivot.
    float rotation = 0;

    bool focused = false;

    bool is_pressed_inside = false;

    Vec2F minimum_size{0};

    Vec2F local_mouse_position;

    AnchorFlag anchor_mode = AnchorFlag::Max;

    void update(double dt) override;

    void input(InputEvent &input_event) override;

    bool is_cursor_inside = false;

    void cursor_entered();

    void cursor_exited();

    /// Visualize the node's size.
    StyleBox debug_size_box;

    MouseFilter mouse_filter = MouseFilter::Stop;

    std::vector<std::function<void()>> callbacks_cursor_entered;
    std::vector<std::function<void()>> callbacks_cursor_exited;
};

} // namespace Flint

#endif // FLINT_NODE_UI_H
