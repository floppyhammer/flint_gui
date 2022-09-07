#include "spin_box.h"

#include "../../../resources/vector_texture.h"
#include "../../../common/geometry.h"

#include <iostream>
#include <iomanip>

namespace Flint {
    SpinBox::SpinBox() {
        type = NodeType::SpinBox;

        theme_normal = std::optional(StyleBox());
        theme_normal->bg_color = ColorU(10, 10, 10);

        theme_focused = std::optional(theme_normal.value());
        theme_focused->border_color = ColorU(200, 200, 200);
        theme_focused->border_width = 2;

        debug_size_box.border_color = ColorU::green();

        // Don't add the label as a child since it's not a normal node but part of the SpinBox.
        label = std::make_shared<Label>("");
        label->set_mouse_filter(MouseFilter::Ignore);
        label->set_horizontal_alignment(Alignment::Center);
        label->set_vertical_alignment(Alignment::Center);
        set_value(0);

        container_v = std::make_shared<VStackContainer>();

        container_h = std::make_shared<HStackContainer>();
        container_h->set_parent(this);
        container_h->add_child(label);
        container_h->add_child(container_v);
        container_h->set_separation(0);
        container_h->set_size(size);
    }

    Vec2<float> SpinBox::calculate_minimum_size() const {
        auto container_size = container_h->calculate_minimum_size();

        return container_size.max(minimum_size);
    }

    void SpinBox::input(std::vector<InputEvent> &input_queue) {
        auto global_position = get_global_position();
        auto active_rect = Rect<float>(global_position, global_position + size);

        for (auto &event: input_queue) {
            bool consume_flag = false;

            if (event.type == InputEventType::MouseMotion) {
                auto args = event.args.mouse_motion;

                if (pressed_inside) {
                    if (drag_to_adjust_value) {
                        set_value(value + args.relative.x * step);
                    }

                    drag_to_adjust_value = true;

                    // Capture cursor when dragging.
                    InputServer::get_singleton()->set_cursor_captured(true);
                }

                if (active_rect.contains_point(args.position)) {
                    consume_flag = true;
                }
            }

            if (event.type == InputEventType::MouseButton) {
                auto args = event.args.mouse_button;

                if (!args.pressed) {
                    if (pressed_inside) {
                        // Release cursor when dragging ends.
                        InputServer::get_singleton()->set_cursor_captured(false);
                    }

                    pressed_inside = false;
                    drag_to_adjust_value = false;
                }

                if (event.is_consumed()) {
                    focused = false;
                    pressed_inside = false;
                } else {
                    if (active_rect.contains_point(args.position)) {
                        if (args.pressed) {
                            focused = true;
                            pressed_inside = true;
                        }

                        consume_flag = true;
                    } else {
                        focused = false;
                    }
                }
            }

            if (consume_flag) {
                event.consume();
            }
        }

        Control::input(input_queue);
    }

    void SpinBox::update(double dt) {
        Control::update(dt);

//        if (icon.has_value()) {
//            auto icon_size = icon.value().size;
//            label->set_size({size.x - icon_size.x, size.y});
//            label->set_position({icon_size.x, 0});
//        } else {
//            label->set_size({size.x, size.y});
//            label->set_position({0, 0});
//        }
        container_h->propagate_update(dt);

        label->update(dt);
    }

    void SpinBox::draw(VkCommandBuffer p_command_buffer) {
        if (!visible) return;

        auto canvas = VectorServer::get_singleton()->canvas;

        auto global_position = get_global_position();

        // Draw bg.
        std::optional<StyleBox> active_style_box;
        active_style_box = focused ? theme_focused : theme_normal;

        if (active_style_box.has_value()) {
            active_style_box.value().add_to_canvas(global_position, size, canvas);
        }

        container_h->propagate_draw(p_command_buffer);

        Control::draw(p_command_buffer);
    }

    void SpinBox::set_position(Vec2<float> p_position) {
        position = p_position;
    }

    void SpinBox::set_size(Vec2<float> p_size) {
        if (size == p_size) return;

        auto path = get_node_path();

        auto final_size = p_size.max(container_h->calculate_minimum_size());
        final_size = final_size.max(minimum_size);

        container_h->set_size(final_size);
        size = final_size;
    }

    void SpinBox::on_focused() {
        for (auto &callback: on_focused_callbacks) {
            callback();
        }
    }

    void SpinBox::connect_signal(std::string signal, std::function<void()> callback) {
        if (signal == "on_focused") {
            on_focused_callbacks.push_back(callback);
        }
    }

    void SpinBox::set_value(float p_value) {
        if (clamped) {
            value = std::clamp(p_value, min_value, max_value);
        } else {
            value = p_value;
        }

        if (is_integer) {
            int32_t value_int = std::round(p_value);
            label->set_text(std::to_string(value_int));
        } else {
            std::ostringstream string_stream;
            string_stream << std::setprecision(rounding_digits) << p_value;
            label->set_text(string_stream.str());
        }
    }
}
