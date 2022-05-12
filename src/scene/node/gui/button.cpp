#include "button.h"

#include "../../../common/math/rect.h"

namespace Flint {
    Button::Button() {
        type = NodeType::Button;

        theme_hovered.border_color = ColorU(200, 200, 200, 255);
        theme_hovered.border_width = 2;

        theme_pressed.bg_color = ColorU(70, 70, 70, 255);
        theme_pressed.border_color = ColorU(200, 200, 200, 255);
        theme_pressed.border_width = 2;

        // Don't add the label as a child since it's not a normal node but part of the button.
        label = std::make_shared<Label>();
        label->set_text("Button");
        label->set_mouse_filter(MouseFilter::IGNORE);
        label->set_horizontal_alignment(Alignment::Center);
        label->set_vertical_alignment(Alignment::Center);
        label->set_parent(this);

        size = label->get_text_size();
        label->set_size(size);

        icon = std::optional(Pathfinder::Shape());
        icon.value().add_circle({}, 8);
    }

    Vec2<float> Button::calculate_minimum_size() {
        auto size = label->calculate_minimum_size();
        if (icon.has_value()) {
            auto icon_size = icon.value().bounds.size();
            size.x += icon_size.x;
        }

        return size.max(minimum_size);
    }

    void Button::input(std::vector<InputEvent> &input_queue) {
        auto global_position = get_global_position();

        for (auto &event: input_queue) {
            if (event.type == InputEventType::MouseMotion) {
                auto args = event.args.mouse_motion;

                if (event.is_consumed()) {
                    hovered = false;
                    pressed = false;
                    pressed_inside = false;
                } else {
                    if (Rect<float>(global_position, global_position + size).contains_point(args.position)) {
                        hovered = true;
                        event.consume();
                    } else {
                        hovered = false;
                        pressed = false;
                        pressed_inside = false;
                    }
                }
            }

            if (event.type == InputEventType::MouseButton) {
                auto args = event.args.mouse_button;

                if (event.is_consumed() && !args.pressed) {
                    if (Rect<float>(global_position, global_position + size).contains_point(args.position)) {
                        pressed = false;
                        pressed_inside = false;
                    }
                } else {
                    if (Rect<float>(global_position, global_position + size).contains_point(args.position)) {
                        pressed = args.pressed;
                        if (pressed) {
                            pressed_inside = true;
                        } else {
                            if (pressed_inside) on_pressed();
                        }
                        event.consume();
                    }
                }
            }
        }

        Control::input(input_queue);
    }

    void Button::update(double dt) {
        Control::update(dt);

        label->update(dt);
        if (icon.has_value()) {
            auto icon_size = icon.value().bounds.size();
            label->set_position({icon_size.x, 0});
        }

        set_size(size.max(calculate_minimum_size()));
    }

    void Button::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton().canvas;

        auto global_position = get_global_position();

        StyleBox active_style_box;
        if (hovered) {
            active_style_box = pressed ? theme_pressed : theme_hovered;
        } else {
            active_style_box = theme_normal;
        }

        active_style_box.add_to_canvas(global_position, size, canvas);

        auto icon_size = icon.value().bounds.size();
        canvas->set_line_width(2);
        canvas->set_transform(Pathfinder::Transform2::from_translation({global_position.x + icon_size.x * 0.5f, global_position.y + size.y * 0.5f}));
        canvas->set_stroke_paint(Pathfinder::Paint::from_color({200, 200, 200, 255}));
        canvas->stroke_shape(icon.value());

        label->draw(p_command_buffer);
    }

    void Button::set_position(Vec2<float> p_position) {
        position = p_position;
    }

    void Button::set_size(Vec2<float> p_size) {
        if (size == p_size) return;

        size = p_size;
        label->set_size(p_size);
        label->need_to_remeasure = true;
    }

    void Button::on_pressed() {
        for (auto &callback: on_pressed_callbacks) {
            callback();
        }
    }

    void Button::connect_signal(std::string signal, std::function<void()> callback) {
        if (signal == "on_pressed") {
            on_pressed_callbacks.push_back(callback);
        }
    }

    void Button::set_text(const std::string &text) {
        label->set_text(text);
    }
}
