#include "control.h"

#include "../sub_viewport.h"
#include "../../../render/swap_chain.h"
#include "../../../resources/default_resource.h"
#include "../../../common/geometry.h"

using Pathfinder::Rect;

namespace Flint {
    Control::Control() {
        type = NodeType::Control;

        debug_size_box.bg_color = ColorU();
        debug_size_box.corner_radius = 0;
        debug_size_box.border_width = 0;
        debug_size_box.border_color = ColorU(202, 130, 94, 255);
    }

    Vec2<float> Control::calculate_minimum_size() const {
        return minimum_size;
    }

    void Control::draw(VkCommandBuffer p_command_buffer) {
        if (size.x > 0 && size.y > 0) {
            debug_size_box.add_to_canvas(get_global_position(), size, VectorServer::get_singleton()->canvas);
        }
    }

    void Control::update(double dt) {

    }

    void Control::input(std::vector<InputEvent> &input_queue) {
        if (mouse_filter != MouseFilter::STOP) return;

        auto global_position = get_global_position();

        auto active_rect = Rect<float>(global_position, global_position + size);

        // Handle mouse input propagation.
        for (auto &event: input_queue) {
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
                }
                    break;
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
                }
                    break;
                default:
                    break;
            }

            if (consume_flag) {
                event.consume();
            }
        }
    }

    Vec2<float> Control::get_global_position() const {
        if (parent != nullptr && parent->extended_from_which_base_node() == NodeType::Control) {
            auto cast_parent = dynamic_cast<Control *>(parent);

            return cast_parent->get_global_position() + position;
        }

        return position;
    }

    void Control::set_mouse_filter(MouseFilter filter) {
        mouse_filter = filter;
    }

    void Control::set_position(Vec2<float> p_position) {
        position = p_position;
    }

    void Control::set_size(Vec2<float> p_size) {
        if (size == p_size) return;

        size = p_size.max(calculate_minimum_size());
    }

    Vec2<float> Control::get_position() const {
        return position;
    }

    Vec2<float> Control::get_size() const {
        return size;
    }

    void Control::set_minimum_size(Vec2<float> p_minimum_size) {
        minimum_size = p_minimum_size;
    }

    Vec2<float> Control::get_minimum_size() const {
        return minimum_size;
    }

    void Control::set_visibility(bool p_visible) {
        visible = p_visible;
    }

    bool Control::get_visibility() const {
        return visible;
    }

    Vec2F Control::get_local_mouse_position() const {
        return local_mouse_position;
    }

    void Control::grab_focus() {
        focused = true;
    }

    void Control::release_focus() {
        focused = false;
    }

    ColorF Control::get_global_modulate() {
        if (parent && parent->extended_from_which_base_node() == NodeType::Control) {
            auto cast_parent = dynamic_cast<Control *>(parent);
            return modulate * cast_parent->get_global_modulate();
        } else {
            return {1, 1, 1, 1};
        }
    }

    bool Control::is_inside_container() const {
        if (parent) {
            switch (parent->type) {
                case NodeType::Container:
                case NodeType::CenterContainer:
                case NodeType::MarginContainer: {
                    return true;
                }
                    break;
                default:
                    return false;
            }
        }
        return false;
    }

    Vec2F Control::get_max_child_min_size() const {
        Vec2F max_child_min_size;

        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                max_child_min_size.max(cast_child->calculate_minimum_size());
            }
        }

        return max_child_min_size;
    }

    void Control::apply_anchor() {
        if (is_inside_container()) return;

        if (parent && parent->extended_from_which_base_node() == NodeType::Control) {
            auto cast_parent = dynamic_cast<Control *>(parent);

            auto actual_size = get_minimum_size().max(size);

            float center_x = (cast_parent->size.x - actual_size.x) * 0.5f;
            float center_y = (cast_parent->size.y - actual_size.y) * 0.5f;
            float right = cast_parent->size.x - actual_size.x;
            float bottom = cast_parent->size.y - actual_size.y;

            switch (anchor_mode) {
                case AnchorFlag::TOP_LEFT: {
                    position = {0, 0};
                }
                    break;
                case AnchorFlag::TOP_RIGHT: {
                    position = {right, 0};
                }
                    break;
                case AnchorFlag::BOTTOM_RIGHT: {
                    position = {right, bottom};
                }
                    break;
                case AnchorFlag::BOTTOM_LEFT: {
                    position = {0, bottom};
                }
                    break;
                case AnchorFlag::CENTER_LEFT: {
                    position = {0, center_y};
                }
                    break;
                case AnchorFlag::CENTER_RIGHT: {
                    position = {right, center_y};
                }
                    break;
                case AnchorFlag::CENTER_TOP: {
                    position = {center_x, 0};
                }
                    break;
                case AnchorFlag::CENTER_BOTTOM: {
                    position = {center_x, bottom};
                }
                    break;
                case AnchorFlag::CENTER: {
                    position = {center_x, center_y};
                }
                    break;
                case AnchorFlag::LEFT_WIDE: {
                    position = {0, 0};
                    size = {actual_size.x, cast_parent->size.y};
                }
                    break;
                case AnchorFlag::RIGHT_WIDE: {
                    position = {right, 0};
                    size = {actual_size.x, cast_parent->size.y};
                }
                    break;
                case AnchorFlag::TOP_WIDE: {
                    position = {0, 0};
                    size = {cast_parent->size.x, actual_size.y};
                }
                    break;
                case AnchorFlag::BOTTOM_WIDE: {
                    position = {0, bottom};
                    size = {cast_parent->size.x, actual_size.y};
                }
                    break;
                case AnchorFlag::VCENTER_WIDE: {
                    position = {center_x, center_y};
                    size = {cast_parent->size.x, actual_size.y};
                }
                    break;
                case AnchorFlag::HCENTER_WIDE: {
                    position = {center_x, center_y};
                    size = {actual_size.x, cast_parent->size.y};
                }
                    break;
                case AnchorFlag::FULL_RECT: {
                    position = {0, 0};
                    size = {cast_parent->size.x, cast_parent->size.y};
                }
                    break;
                case AnchorFlag::MAX: {
                    return;
                }
            }
        }
    }

    void Control::cursor_entered() {

    }

    void Control::cursor_exited() {

    }

    void Control::set_anchor_flag(AnchorFlag anchor_flag) {
        if (anchor_flag == anchor_mode) return;

        anchor_mode = anchor_flag;
        apply_anchor();
    }

    void Control::get_anchor_flag() {
        return;
        anchor_mode;
    }
}
