#include "button.h"

#include "../../common/geometry.h"
#include "../../resources/default_resource.h"
#include "../../resources/vector_texture.h"

namespace Flint::Scene {

Button::Button() {
    type = NodeType::Button;

    theme_normal = DefaultResource::get_singleton()->get_default_theme()->button.styles["normal"];

    theme_hovered = DefaultResource::get_singleton()->get_default_theme()->button.styles["hovered"];

    theme_pressed = DefaultResource::get_singleton()->get_default_theme()->button.styles["pressed"];

    debug_size_box.border_color = ColorU::green();

    // Don't add the label as a child since it's not a normal node but part of the button.
    label = std::make_shared<Label>("Button");
    label->set_mouse_filter(MouseFilter::Ignore);
    label->set_horizontal_alignment(Alignment::Center);
    label->set_vertical_alignment(Alignment::Center);
    label->theme_background = StyleBox::from_empty();

    icon_rect = std::make_shared<TextureRect>();
    icon_rect->set_stretch_mode(TextureRect::StretchMode::KeepCentered);

    hstack_container = std::make_shared<HStackContainer>();
    hstack_container->add_child(icon_rect);
    hstack_container->add_child(label);
    hstack_container->set_separation(0);

    margin_container = std::make_shared<MarginContainer>();
    margin_container->set_margin_all(0);
    margin_container->add_child(hstack_container);
    margin_container->set_parent(this);
    margin_container->set_size(size);
}

Vec2F Button::calc_minimum_size() const {
    auto container_size = margin_container->calc_minimum_size();

    return container_size.max(minimum_size);
}

void Button::input(InputEvent &event) {
    auto global_position = get_global_position();

    bool consume_flag = false;

    if (event.type == InputEventType::MouseMotion) {
        auto args = event.args.mouse_motion;

        if (event.is_consumed()) {
            hovered = false;
            if (!toggle_mode) {
                pressed = false;
            }
            pressed_inside = false;
        } else {
            if (RectF(global_position, global_position + size).contains_point(args.position)) {
                hovered = true;
                consume_flag = true;
            } else {
                hovered = false;
                if (!toggle_mode) {
                    pressed = false;
                }
                pressed_inside = false;
            }
        }
    }

    if (event.type == InputEventType::MouseButton) {
        auto args = event.args.mouse_button;

        if (event.is_consumed()) {
            if (!args.pressed) {
                if (RectF(global_position, global_position + size).contains_point(args.position)) {
                    if (!toggle_mode) {
                        pressed = false;
                        pressed_inside = false;
                    }
                }
            }
        } else {
            if (RectF(global_position, global_position + size).contains_point(args.position)) {
                if (!toggle_mode) {
                    pressed = args.pressed;
                    if (pressed) {
                        pressed_inside = true;
                    } else {
                        if (pressed_inside) {
                            when_pressed();
                        }
                    }
                } else {
                    if (args.pressed) {
                        pressed_inside = true;
                    } else {
                        if (pressed_inside && !pressed) {
                            pressed = true;
                            when_pressed();
                        }
                    }
                }

                consume_flag = true;
            }
        }
    }

    if (consume_flag) {
        event.consume();
    }

    NodeUi::input(event);
}

void Button::update(double dt) {
    NodeUi::update(dt);

    margin_container->propagate_update(dt);
}

void Button::draw() {
    if (!visible) {
        return;
    }

    auto vector_server = VectorServer::get_singleton();

    auto global_position = get_global_position();

    // Draw bg.
    std::optional<StyleBox> active_style_box;
    if (pressed) {
        active_style_box = theme_pressed;
    } else if (hovered) {
        active_style_box = theme_hovered;
    } else {
        active_style_box = theme_normal;
    }

    active_style_box->bg_color = ColorU(active_style_box->bg_color.to_f32() * modulate.to_f32());
    active_style_box->border_color = ColorU(active_style_box->border_color.to_f32() * modulate.to_f32());

    if (active_style_box.has_value()) {
        vector_server->draw_style_box(active_style_box.value(), global_position, size);
    }

    margin_container->propagate_draw(VK_NULL_HANDLE, VK_NULL_HANDLE);

    NodeUi::draw();
}

void Button::set_position(Vec2F p_position) {
    position = p_position;
}

void Button::set_size(Vec2F p_size) {
    if (size == p_size) return;

    auto path = get_node_path();

    auto final_size = p_size.max(margin_container->calc_minimum_size());
    final_size = final_size.max(minimum_size);

    margin_container->set_size(final_size);
    size = final_size;
}

void Button::when_pressed() {
    for (auto &callback : pressed_callbacks) {
        callback();
    }
}

void Button::connect_signal(const std::string &signal, const std::function<void()> &callback) {
    NodeUi::connect_signal(signal, callback);

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

void Button::set_expand_icon(bool enable) {
    if (enable) {
        icon_rect->container_sizing.expand_h = true;
        icon_rect->container_sizing.flag_h = ContainerSizingFlag::Fill;
    } else {
        icon_rect->container_sizing.expand_h = false;
    }
}

void Button::set_toggle_mode(bool enable) {
    toggle_mode = enable;
}

void ButtonGroup::update() {
    // We should not trigger any button signals when changing their states from ButtonGroup.
    for (auto &b : buttons) {
        if (b.lock() == pressed_button.lock()) {
            b.lock()->pressed = true;
        } else {
            b.lock()->pressed = false;
        }
    }
}

void ButtonGroup::add_button(const std::weak_ptr<Button> &new_button) {
    buttons.push_back(new_button);

    auto callback = [this, new_button] { this->pressed_button = new_button; };
    new_button.lock()->connect_signal("pressed", callback);
}

} // namespace Flint
