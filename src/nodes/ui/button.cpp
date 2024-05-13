#include "button.h"

#include <optional>

#include "../../common/geometry.h"
#include "../../resources/default_resource.h"
#include "../../resources/vector_image.h"

namespace Flint {

Button::Button() {
    type = NodeType::Button;

    auto default_theme = DefaultResource::get_singleton()->get_default_theme();

    theme_normal = default_theme->button.styles["normal"];

    theme_hovered = default_theme->button.styles["hovered"];

    theme_pressed = default_theme->button.styles["pressed"];

    debug_size_box.border_color = ColorU::green();

    // Don't add the label as a child since it's not a normal node but part of the button.
    label = std::make_shared<Label>();
    label->set_text("Button");
    label->set_mouse_filter(MouseFilter::Ignore);
    label->set_horizontal_alignment(Alignment::Center);
    label->set_vertical_alignment(Alignment::Center);
    label->set_text_style(TextStyle{default_theme->button.colors["text"]});
    label->theme_background = StyleBox::from_empty();

    icon_rect = std::make_shared<TextureRect>();
    icon_rect->set_stretch_mode(TextureRect::StretchMode::KeepCentered);
    icon_rect->set_mouse_filter(MouseFilter::Ignore);

    hbox_container = std::make_shared<HBoxContainer>();
    hbox_container->add_child(icon_rect);
    hbox_container->add_child(label);
    hbox_container->set_separation(2);
    hbox_container->set_mouse_filter(MouseFilter::Ignore);

    margin_container = std::make_shared<MarginContainer>();
    margin_container->set_margin_all(4);
    margin_container->add_child(hbox_container);
    margin_container->set_size(size);
    margin_container->set_mouse_filter(MouseFilter::Ignore);

    add_embedded_child(margin_container);

    callbacks_cursor_entered.emplace_back(
        [this] { InputServer::get_singleton()->set_cursor(this->get_window(), CursorShape::Hand); });

    callbacks_cursor_exited.emplace_back(
        [this] { InputServer::get_singleton()->set_cursor(this->get_window(), CursorShape::Arrow); });
}

void Button::calc_minimum_size() {
    auto container_size = margin_container->get_effective_minimum_size();

    calculated_minimum_size = container_size;
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
                        if (pressed_inside) {
                            if (pressed) {
                                pressed = false;
                            } else {
                                pressed = true;
                                when_pressed();
                            }

                            when_toggled(pressed);
                        }
                    }
                }

                consume_flag = true;
            }
        }
    }

    NodeUi::input(event);

    if (consume_flag) {
        event.consume();
    }
}

void Button::update(double dt) {
    NodeUi::update(dt);

    // std::vector<Node *> descendants;
    // dfs_preorder_ltr_traversal(margin_container.get(), descendants);
    // for (auto &node : descendants) {
    //     node->update(dt);
    // }
}

void Button::draw() {
    if (!visible_) {
        return;
    }

    auto vector_server = VectorServer::get_singleton();

    auto global_position = get_global_position();

    icon_rect->set_texture(icon_normal_);

    // Draw bg.
    std::optional<StyleBox> active_style_box;
    if (pressed) {
        active_style_box = theme_pressed;

        // When toggled, hovering the button should show the hover effect.
        if (toggle_mode && hovered) {
            active_style_box = theme_hovered;
        }

        if (icon_pressed_) {
            icon_rect->set_texture(icon_pressed_);
        }
    } else if (hovered) {
        active_style_box = theme_hovered;
    } else {
        active_style_box = theme_normal;
    }

    active_style_box->bg_color = ColorU(active_style_box->bg_color.to_f32() * modulate.to_f32());
    active_style_box->border_color = ColorU(active_style_box->border_color.to_f32() * modulate.to_f32());

    if (active_style_box.has_value() && !flat_) {
        vector_server->draw_style_box(active_style_box.value(), global_position, size);
    }

    NodeUi::draw();
}

void Button::set_position(Vec2F new_position) {
    position = new_position;
}

void Button::set_size(Vec2F p_size) {
    if (size == p_size) {
        return;
    }

    auto path = get_node_path();

    auto final_size = p_size.max(margin_container->get_effective_minimum_size());
    final_size = final_size.max(custom_minimum_size);

    margin_container->set_size(final_size);
    size = final_size;
}

void Button::when_pressed() {
    for (auto &callback : pressed_callbacks) {
        callback();
    }
}

void Button::when_toggled(bool pressed) {
    for (auto &callback : toggled_callbacks) {
        callback(pressed);
    }
}

void Button::connect_signal(const std::string &signal, const std::function<void()> &callback) {
    NodeUi::connect_signal(signal, callback);

    if (signal == "pressed") {
        pressed_callbacks.push_back(callback);
    }
}

void Button::connect_signal_toggled(const std::function<void(bool)> &callback) {
    toggled_callbacks.push_back(callback);
}

void Button::set_text(const std::string &text) {
    label->set_text(text);
}

void Button::set_icon_normal(const std::shared_ptr<Image> &icon) {
    icon_normal_ = icon;
}

void Button::set_icon_pressed(const std::shared_ptr<Image> &icon) {
    icon_pressed_ = icon;
}

void Button::set_icon_expand(bool enable) {
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
