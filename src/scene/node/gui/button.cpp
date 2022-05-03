#include "button.h"

#include "../../../common/math/rect.h"

namespace Flint {
    Button::Button() {
        type = NodeType::Button;

        theme_hovered.border_color = ColorU(200, 200, 200, 255);

        theme_pressed.bg_color = ColorU(70, 70, 70, 255);
        theme_pressed.border_color = ColorU(200, 200, 200, 255);
    }

    void Button::input(std::vector<InputEvent> &input_queue) {
        for (auto &event : input_queue) {
            if (event.type == InputEventType::MouseMotion) {
                auto args = event.args.mouse_motion;

                if (Rect<float>(position, position + size).contains_point(args.position)) {
                    hovered = true;
                } else {
                    hovered = false;
                }
            }

            if (event.type == InputEventType::MouseButton) {
                auto args = event.args.mouse_button;

                if (Rect<float>(position, position + size).contains_point(args.position)) {
                    pressed = args.pressed;
                }
            }
        }

        Control::input(input_queue);
    }

    void Button::update(double dt) {
        Control::update(dt);
    }

    void Button::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton().canvas;

        StyleBox active_style_box;
        if (hovered) {
            active_style_box = pressed ? theme_pressed : theme_hovered;
        } else {
            active_style_box = theme_normal;
        }

        active_style_box.add_to_canvas(position, size, canvas);
    }
}
