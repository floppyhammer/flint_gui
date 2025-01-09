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

/// How a UI node handles mouse input propagation.
enum class MouseFilter {
    Stop,   // Use input and mark it as consumed.
    Pass,   // Use input and don't mark it as consumed.
    Ignore, // Ignore input.
};

/// Anchor takes effect only when a UI node is not a child of a container.
enum class AnchorFlag {
    None,

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

enum class ContainerSizingFlag {
    Fill,         // Occupy the full space in the grow direction.
    ShrinkStart,  // Shrink to the minimum size at the start in the grow direction.
    ShrinkCenter, // Shrink to the minimum size at the center in the grow direction.
    ShrinkEnd,    // Shrink to the minimum size at the end in the grow direction.
};

/// How a parent container organizes this UI node.
struct ContainerSizing {
    // Control how the size changes in the horizontal direction.
    ContainerSizingFlag flag_h = ContainerSizingFlag::Fill;
    // Expand position in the horizontal direction, but not changing size.
    bool expand_h = false;
    // Control how the size changes in the vertical direction.
    ContainerSizingFlag flag_v = ContainerSizingFlag::Fill;
    // Expand position in the vertical direction, but not changing size.
    bool expand_v = false;
};

class NodeUi : public Node {
public:
    NodeUi();

    virtual void set_position(Vec2F new_position);

    virtual Vec2F get_position() const;

    virtual void set_size(Vec2F new_size);

    virtual Vec2F get_size() const;

    virtual void set_custom_minimum_size(Vec2F new_size);

    virtual Vec2F get_custom_minimum_size() const;

    Vec2F get_effective_minimum_size() const;

    /// Runs once per frame.
    virtual void calc_minimum_size();

    /// Only for secondary (off-tree) nodes.
    void calc_minimum_size_recursively();

    bool is_ui_node() const override {
        return true;
    }

    Vec2F get_global_position() const;

    void calc_global_position(Vec2F parent_global_position);

    virtual bool ignore_mouse_input_outside_rect() const {
        return false;
    }

    virtual void draw();

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

    AnchorFlag get_anchor_flag() const;

    void when_parent_size_changed(Vec2F new_size) override;

    void when_cursor_entered();

    void when_cursor_exited();

protected:
    Vec2F position{0};
    Vec2F size{1};
    Vec2F scale{1};
    Vec2F pivot_offset{0}; // Top-left as the default pivot.
    float rotation = 0;

    Vec2F calculated_glocal_position{0};

    bool layout_is_dirty = true;

    bool focused = false;

    bool is_pressed_inside = false;

    Vec2F custom_minimum_size{};

    /// Minimum size with all elements considered. Differs from user set `minimum_size`.
    Vec2F calculated_minimum_size{};

    Vec2F local_mouse_position;

    AnchorFlag anchor_mode = AnchorFlag::None;

    bool is_cursor_inside = false;

    void update(double dt) override;

    void input(InputEvent &input_event) override;

    void cursor_entered();

    void cursor_exited();

    /// Visualize the node's size.
    StyleBox debug_size_box;

    MouseFilter mouse_filter = MouseFilter::Stop;

    std::vector<AnyCallable<void>> callbacks_cursor_entered;
    std::vector<AnyCallable<void>> callbacks_cursor_exited;
    std::vector<std::function<void(Vec2F size)>> callbacks_parent_size_changed;
};

} // namespace Flint

#endif // FLINT_NODE_UI_H
