#include "node_ui.h"

#include "../../common/geometry.h"
#include "../../resources/default_resource.h"
#include "../scene_tree.h"

using Pathfinder::Rect;

namespace Flint {

NodeUi::NodeUi() {
    type = NodeType::NodeUi;

    debug_size_box.bg_color = ColorU();
    debug_size_box.border_color = ColorU::red();
    debug_size_box.border_width = 1;
    debug_size_box.corner_radius = 0;
}

/// Runs once per frame.
void NodeUi::calc_minimum_size() {
    calculated_minimum_size = {};
}

void NodeUi::calc_minimum_size_recursively() {
    std::vector<Node *> secondary_nodes;
    dfs_postorder_ltr_traversal(this, secondary_nodes);
    for (auto &node : secondary_nodes) {
        if (node->is_ui_node()) {
            auto ui_node = dynamic_cast<NodeUi *>(node);
            ui_node->calc_minimum_size();

            // Debug.
            // auto effective_minimum_size = ui_node->get_effective_minimum_size();
            // auto child_name = get_node_type_name(ui_node->type);
            // auto _ = 0;
        }
    }

    // After we get all children's minimum sizes, we calculate it own minimum size.
    calc_minimum_size();
}

Vec2F NodeUi::get_effective_minimum_size() const {
    // Take both custom_minimum_size and calculated_minimum_size into account.
    return custom_minimum_size.max(calculated_minimum_size);
}

void NodeUi::draw() {
    Node::draw();
#ifdef FLINT_GUI_VISUAL_DEBUG
    if (size.x > 0 && size.y > 0) {
        auto vector_server = VectorServer::get_singleton();
        vector_server->draw_style_box(debug_size_box, get_global_position(), size);
    }
#endif
}

void NodeUi::update(double dt) {
    apply_anchor();

    size = get_effective_minimum_size().max(size);

    Node::update(dt);
}

void NodeUi::input(InputEvent &event) {
    if (mouse_filter == MouseFilter::Ignore) {
        return;
    }

    auto global_position = get_global_position();

    auto active_rect = RectF(global_position, global_position + size);

    // Handle mouse input propagation.
    bool consume_flag = false;

    switch (event.type) {
        case InputEventType::MouseMotion: {
            // Mouse position relative to the node's origin.
            local_mouse_position = event.args.mouse_motion.position - global_position;

            if (active_rect.contains_point(event.args.mouse_motion.position)) {
                if (!event.is_consumed()) {
                    is_cursor_inside = true;
                    cursor_entered();
                }

                consume_flag = true;
            } else {
                if (is_cursor_inside) {
                    is_cursor_inside = false;
                    cursor_exited();
                }
            }
        } break;
        case InputEventType::MouseButton: {
            auto args = event.args.mouse_button;

            if (args.pressed) {
                if (active_rect.contains_point(args.position)) {
                    grab_focus();

                    consume_flag = true;
                } else {
                    release_focus();
                }
            }
        } break;
        default:
            break;
    }

    if (consume_flag) {
        event.consume();
    }

    Node::input(event);
}

Vec2F NodeUi::get_global_position() const {
    return calculated_global_position;
}

void NodeUi::calc_global_position(Vec2F parent_global_position) {
    calculated_global_position = parent_global_position + position;
}

void NodeUi::set_mouse_filter(MouseFilter filter) {
    mouse_filter = filter;
}

void NodeUi::set_position(Vec2F new_position) {
    position = new_position;
}

void NodeUi::set_size(Vec2F new_size) {
    if (size == new_size) {
        return;
    }

    size = new_size.max(get_effective_minimum_size());
}

Vec2F NodeUi::get_position() const {
    return position;
}

Vec2F NodeUi::get_size() const {
    return size;
}

void NodeUi::set_custom_minimum_size(Vec2F new_size) {
    custom_minimum_size = new_size;
}

Vec2F NodeUi::get_custom_minimum_size() const {
    return custom_minimum_size;
}

Vec2F NodeUi::get_local_mouse_position() const {
    return local_mouse_position;
}

void NodeUi::grab_focus() {
    focused = true;
}

void NodeUi::release_focus() {
    for (auto &callback : callbacks_focus_released) {
        try {
            callback();
        } catch (std::bad_any_cast &) {
            Logger::error("Mismatched signal argument types!");
        }
    }

    focused = false;
}

ColorU NodeUi::get_global_modulate() {
    if (parent && parent->is_ui_node()) {
        auto cast_parent = dynamic_cast<NodeUi *>(parent);
        return ColorU(modulate.to_f32() * cast_parent->get_global_modulate().to_f32());
    } else {
        return ColorU::white();
    }
}

bool NodeUi::is_inside_container() const {
    if (parent) {
        switch (parent->get_node_type()) {
            case NodeType::Container:
            case NodeType::CenterContainer:
            case NodeType::CollapseContainer:
            case NodeType::MarginContainer:
            case NodeType::HBoxContainer:
            case NodeType::VBoxContainer:
            case NodeType::ScrollContainer:
            case NodeType::TabContainer: {
                return true;
            } break;
            default:
                return false;
        }
    }
    return false;
}

Vec2F NodeUi::get_max_child_min_size() const {
    Vec2F max_child_min_size;

    for (auto &child : children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            max_child_min_size = max_child_min_size.max(cast_child->get_effective_minimum_size());
        }
    }

    return max_child_min_size;
}

void NodeUi::apply_anchor() {
    if (is_inside_container()) {
        return;
    }

    Vec2F parent_size;

    // If no parent or the parent is not a UI node, use the parent window's size for anchoring.
    if (parent && parent->is_ui_node()) {
        auto ui_parent = dynamic_cast<NodeUi *>(parent);
        parent_size = ui_parent->get_size();
    } else {
        auto render_server = RenderServer::get_singleton();
        auto window = render_server->window_builder_->get_window(get_window_index());

        parent_size = window.lock()->get_logical_size().to_f32();
    }

    auto actual_size = get_effective_minimum_size().max(size);

    float center_x = (parent_size.x - actual_size.x) * 0.5f;
    float center_y = (parent_size.y - actual_size.y) * 0.5f;
    float right = parent_size.x - actual_size.x;
    float bottom = parent_size.y - actual_size.y;

    switch (anchor_mode) {
        case AnchorFlag::None: {
            // Do nothing.
        } break;
        case AnchorFlag::TopLeft: {
            position = {0, 0};
        } break;
        case AnchorFlag::TopRight: {
            position = {right, 0};
        } break;
        case AnchorFlag::BottomRight: {
            position = {right, bottom};
        } break;
        case AnchorFlag::BottomLeft: {
            position = {0, bottom};
        } break;
        case AnchorFlag::CenterLeft: {
            position = {0, center_y};
        } break;
        case AnchorFlag::CenterRight: {
            position = {right, center_y};
        } break;
        case AnchorFlag::CenterTop: {
            position = {center_x, 0};
        } break;
        case AnchorFlag::CenterBottom: {
            position = {center_x, bottom};
        } break;
        case AnchorFlag::Center: {
            position = {center_x, center_y};
        } break;
        case AnchorFlag::LeftWide: {
            position = {0, 0};
            size = {actual_size.x, parent_size.y};
        } break;
        case AnchorFlag::RightWide: {
            position = {right, 0};
            size = {actual_size.x, parent_size.y};
        } break;
        case AnchorFlag::TopWide: {
            position = {0, 0};
            size = {parent_size.x, actual_size.y};
        } break;
        case AnchorFlag::BottomWide: {
            position = {0, bottom};
            size = {parent_size.x, actual_size.y};
        } break;
        case AnchorFlag::VCenterWide: {
            position = {center_x, center_y};
            size = {parent_size.x, actual_size.y};
        } break;
        case AnchorFlag::HCenterWide: {
            position = {center_x, center_y};
            size = {actual_size.x, parent_size.y};
        } break;
        case AnchorFlag::FullRect: {
            position = {0, 0};
            size = {parent_size.x, parent_size.y};
        } break;
        case AnchorFlag::Max: {
            abort();
        }
    }
}

void NodeUi::cursor_entered() {
    for (auto &callback : callbacks_cursor_entered) {
        callback();
    }
}

void NodeUi::cursor_exited() {
    for (auto &callback : callbacks_cursor_exited) {
        callback();
    }
}

void NodeUi::set_anchor_flag(AnchorFlag anchor_flag) {
    if (anchor_flag == anchor_mode) {
        return;
    }

    anchor_mode = anchor_flag;
}

AnchorFlag NodeUi::get_anchor_flag() const {
    return anchor_mode;
}

void NodeUi::when_parent_size_changed(Vec2F new_size) {
    for (auto &child : children) {
        if (child->get_node_type() == NodeType::NodeUi) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            cast_child->when_parent_size_changed(size);
        }
    }
}

void NodeUi::connect_signal(const std::string &signal, const AnyCallable<void> &callback) {
    Node::connect_signal(signal, callback);

    if (signal == "focus_released") {
        callbacks_focus_released.push_back(callback);
    }
}

} // namespace Flint
