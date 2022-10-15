#include "button.h"

#include "../../../common/geometry.h"
#include "../../../resources/vector_texture.h"

namespace Flint {
Button::Button() {
    type = NodeType::Button;

    theme_normal = std::optional(StyleBox());
    theme_normal.value().bg_color = ColorU(27, 27, 27, 255);
    theme_normal.value().border_color = ColorU(100, 100, 100, 255);
    theme_normal.value().border_width = 2;

    theme_hovered = std::optional(StyleBox());
    theme_hovered.value().border_color = ColorU(163, 163, 163, 255);
    theme_hovered.value().border_width = 2;

    theme_pressed = std::optional(StyleBox());
    theme_pressed.value().bg_color = ColorU(70, 70, 70, 255);
    theme_pressed.value().border_color = ColorU(163, 163, 163, 255);
    theme_pressed.value().border_width = 2;

    debug_size_box.border_color = ColorU::green();

    // Don't add the label as a child since it's not a normal node but part of the button.
    label = std::make_shared<Label>("Button");
    label->set_mouse_filter(MouseFilter::Ignore);
    label->set_horizontal_alignment(Alignment::Center);
    label->set_vertical_alignment(Alignment::Center);

    auto vector_texture = VectorTexture::from_empty(24, 24);
    VectorPath vp;
    vp.path2d.add_circle({vector_texture->get_width() * 0.5f, vector_texture->get_height() * 0.5f}, 8);
    vp.stroke_color = ColorU(163, 163, 163, 255);
    vp.stroke_width = 2;
    vector_texture->add_path(vp);
    icon_rect = std::make_shared<TextureRect>();
    icon_rect->set_texture(vector_texture);

    hbox_container = std::make_shared<HStackContainer>();
    hbox_container->add_child(icon_rect);
    hbox_container->add_child(label);
    hbox_container->set_separation(0);

    margin_container = std::make_shared<MarginContainer>();
    margin_container->set_margin_all(4);
    margin_container->add_child(hbox_container);
    margin_container->set_parent(this);
    margin_container->set_size(size);
}

Vec2<float> Button::calculate_minimum_size() const {
    auto container_size = margin_container->calculate_minimum_size();

    return container_size.max(minimum_size);
}

void Button::input(std::vector<InputEvent> &input_queue) {
    auto global_position = get_global_position();

    for (auto &event : input_queue) {
        bool consume_flag = false;

        if (event.type == InputEventType::MouseMotion) {
            auto args = event.args.mouse_motion;

            if (event.is_consumed()) {
                hovered = false;
                pressed = false;
                pressed_inside = false;
            } else {
                if (Rect<float>(global_position, global_position + size).contains_point(args.position)) {
                    hovered = true;
                    consume_flag = true;
                } else {
                    hovered = false;
                    pressed = false;
                    pressed_inside = false;
                }
            }
        }

        if (event.type == InputEventType::MouseButton) {
            auto args = event.args.mouse_button;

            if (event.is_consumed()) {
                if (!args.pressed) {
                    if (Rect<float>(global_position, global_position + size).contains_point(args.position)) {
                        pressed = false;
                        pressed_inside = false;
                    }
                }
            } else {
                if (Rect<float>(global_position, global_position + size).contains_point(args.position)) {
                    pressed = args.pressed;
                    if (pressed) {
                        pressed_inside = true;
                    } else {
                        if (pressed_inside) when_pressed();
                    }
                    consume_flag = true;
                }
            }
        }

        if (consume_flag) {
            event.consume();
        }
    }

    Control::input(input_queue);
}

void Button::update(double dt) {
    Control::update(dt);

    margin_container->propagate_update(dt);
}

void Button::draw(VkCommandBuffer p_command_buffer) {
    if (!visible) return;

    auto canvas = VectorServer::get_singleton()->canvas;

    auto global_position = get_global_position();

    // Draw bg.
    std::optional<StyleBox> active_style_box;
    if (hovered) {
        active_style_box = pressed ? theme_pressed : theme_hovered;
    } else {
        active_style_box = theme_normal;
    }

    if (active_style_box.has_value()) {
        active_style_box.value().add_to_canvas(global_position, size, canvas);
    }

    margin_container->propagate_draw(p_command_buffer);

    Control::draw(p_command_buffer);
}

void Button::set_position(Vec2<float> p_position) {
    position = p_position;
}

void Button::set_size(Vec2<float> p_size) {
    if (size == p_size) return;

    auto path = get_node_path();

    auto final_size = p_size.max(margin_container->calculate_minimum_size());
    final_size = final_size.max(minimum_size);

    margin_container->set_size(final_size);
    size = final_size;
}

void Button::when_pressed() {
    for (auto &callback : pressed_callbacks) {
        callback();
    }
}

void Button::connect_signal(std::string signal, const std::function<void()> &callback) {
    if (signal == "pressed") {
        pressed_callbacks.push_back(callback);
    }
}

void Button::set_text(const std::string &text) {
    label->set_text(text);
}

void Button::set_icon(const std::shared_ptr<Texture> &p_icon) {
    icon_rect->set_texture(p_icon);
}
} // namespace Flint
