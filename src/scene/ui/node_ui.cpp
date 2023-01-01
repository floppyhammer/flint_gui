#include "node_ui.h"

#include "../../common/geometry.h"
#include "../../render/swap_chain.h"
#include "../../resources/default_resource.h"
#include "../world.h"

using Pathfinder::Rect;

namespace Flint {

NodeUi::NodeUi() {
    type = NodeType::NodeUi;

    debug_size_box.bg_color = ColorU();
    debug_size_box.border_color = ColorU::red();
    debug_size_box.border_width = 0;
    debug_size_box.corner_radius = 0;
}

Vec2F NodeUi::calc_minimum_size() const {
    return minimum_size;
}

void NodeUi::propagate_draw(VkCommandBuffer cmd_buffer) {
    if (!visible) {
        return;
    }

    draw();

    for (auto &child : children) {
        child->propagate_draw(cmd_buffer);
    }
}

void NodeUi::draw() {
    if (size.x > 0 && size.y > 0) {
        auto vector_server = VectorServer::get_singleton();
        vector_server->draw_style_box(debug_size_box, get_global_position(), size);
    }
}

void NodeUi::update(double dt) {
    if (visual_debug) {
        debug_size_box.border_width = 2;
    } else {
        debug_size_box.border_width = 0;
    }

    apply_anchor();
}

void NodeUi::input(InputEvent &event) {
    if (mouse_filter != MouseFilter::Stop) {
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
}

Vec2F NodeUi::get_global_position() const {
    if (parent != nullptr && parent->is_ui_node()) {
        auto cast_parent = dynamic_cast<NodeUi *>(parent);

        return cast_parent->get_global_position() + position;
    }

    return position;
}

void NodeUi::set_mouse_filter(MouseFilter filter) {
    mouse_filter = filter;
}

void NodeUi::set_position(Vec2F p_position) {
    position = p_position;
}

void NodeUi::set_size(Vec2F p_size) {
    if (size == p_size) {
        return;
    }

    size = p_size.max(calc_minimum_size());
}

Vec2F NodeUi::get_position() const {
    return position;
}

Vec2F NodeUi::get_size() const {
    return size;
}

void NodeUi::set_minimum_size(Vec2F p_minimum_size) {
    minimum_size = p_minimum_size;
}

Vec2F NodeUi::get_minimum_size() const {
    return minimum_size;
}

Vec2F NodeUi::get_local_mouse_position() const {
    return local_mouse_position;
}

void NodeUi::grab_focus() {
    focused = true;
}

void NodeUi::release_focus() {
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
            case NodeType::MarginContainer: {
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
            max_child_min_size.max(cast_child->calc_minimum_size());
        }
    }

    return max_child_min_size;
}

void NodeUi::apply_anchor() {
    if (is_inside_container()) {
        return;
    }

    if (parent && parent->is_ui_node()) {
        auto cast_parent = dynamic_cast<NodeUi *>(parent);

        auto actual_size = get_minimum_size().max(size);

        float center_x = (cast_parent->size.x - actual_size.x) * 0.5f;
        float center_y = (cast_parent->size.y - actual_size.y) * 0.5f;
        float right = cast_parent->size.x - actual_size.x;
        float bottom = cast_parent->size.y - actual_size.y;

        switch (anchor_mode) {
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
                size = {actual_size.x, cast_parent->size.y};
            } break;
            case AnchorFlag::RightWide: {
                position = {right, 0};
                size = {actual_size.x, cast_parent->size.y};
            } break;
            case AnchorFlag::TopWide: {
                position = {0, 0};
                size = {cast_parent->size.x, actual_size.y};
            } break;
            case AnchorFlag::BottomWide: {
                position = {0, bottom};
                size = {cast_parent->size.x, actual_size.y};
            } break;
            case AnchorFlag::VCenterWide: {
                position = {center_x, center_y};
                size = {cast_parent->size.x, actual_size.y};
            } break;
            case AnchorFlag::HCenterWide: {
                position = {center_x, center_y};
                size = {actual_size.x, cast_parent->size.y};
            } break;
            case AnchorFlag::FullRect: {
                position = {0, 0};
                size = {cast_parent->size.x, cast_parent->size.y};
            } break;
            case AnchorFlag::Max: {
                return;
            }
        }
    }
}

void NodeUi::cursor_entered() {
}

void NodeUi::cursor_exited() {
}

void NodeUi::set_anchor_flag(AnchorFlag anchor_flag) {
    if (anchor_flag == anchor_mode) {
        return;
    }

    anchor_mode = anchor_flag;
    apply_anchor();
}

void NodeUi::get_anchor_flag() {
    return;
    anchor_mode;
}

void NodeUi::when_window_size_changed(Vec2I new_size) {
    if (parent->get_node_type() != NodeType::UiLayer) {
        return;
    }

    switch (anchor_mode) {
        case AnchorFlag::FullRect: {
            size = new_size.to_f32();
        } break;
        case AnchorFlag::TopRight: {
            position.x = new_size.x - size.x;
        } break;
        case AnchorFlag::BottomLeft: {
            position.y = new_size.y - size.y;
        } break;
        case AnchorFlag::BottomRight: {
            position = new_size.to_f32() - size;
        } break;
        case AnchorFlag::CenterLeft: {
            position.y = new_size.y * 0.5 - size.y * 0.5;
        } break;
        case AnchorFlag::CenterRight: {
            position.x = new_size.x - size.x;
            position.y = new_size.y * 0.5 - size.y * 0.5;
        } break;
        case AnchorFlag::CenterTop: {
            position.x = new_size.x * 0.5 - size.x * 0.5;
        } break;
        case AnchorFlag::CenterBottom:
            break;
        case AnchorFlag::Center:
            break;
        case AnchorFlag::LeftWide:
            break;
        case AnchorFlag::RightWide:
            break;
        case AnchorFlag::TopWide:
            break;
        case AnchorFlag::BottomWide:
            break;
        case AnchorFlag::VCenterWide:
            break;
        case AnchorFlag::HCenterWide:
            break;
        case AnchorFlag::Max:
        default:
            break;
    }
}

} // namespace Flint
