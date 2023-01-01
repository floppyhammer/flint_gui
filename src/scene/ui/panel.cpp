#include "panel.h"

#include <string>

#include "../../common/logger.h"

namespace Flint {

Panel::Panel() {
    type = NodeType::Panel;

    theme_panel = std::make_optional(StyleBox());
    theme_panel.value().bg_color = ColorU(27, 27, 27, 255);
    theme_panel.value().border_color = {75, 75, 75, 100};
    theme_panel.value().border_width = 3;
    theme_panel.value().corner_radius = 8;

    // Set up title bar.
    // ---------------------------------------------------------
    theme_title_bar_line = std::make_optional(StyleLine());
    theme_title_bar_line.value().color = {50, 50, 50, 255};

    collapsed_tex = std::make_shared<VectorTexture>("../assets/icons/ArrowRight.svg");
    expanded_tex = std::make_shared<VectorTexture>("../assets/icons/ArrowDown.svg");
    close_tex = std::make_shared<VectorTexture>("../assets/icons/Close.svg");

    {
        title_label = std::make_shared<Label>("Panel");
        title_label->set_horizontal_alignment(Alignment::Center);
        title_label->set_vertical_alignment(Alignment::Center);
        title_label->set_mouse_filter(MouseFilter::Ignore);
        title_label->container_sizing.expand_h = true;
        title_label->container_sizing.flag_h = ContainerSizingFlag::Fill;
    }

    {
        close_button = std::make_shared<Button>();
        close_button->set_text("");
        close_button->set_parent(this);
        close_button->set_icon(close_tex);
        close_button->set_minimum_size(Vec2F(title_bar_height));
        close_button->set_expand_icon(true);
        close_button->theme_normal.border_width = 0.0;
        close_button->theme_normal.bg_color = ColorU::transparent_black();
    }

    collapse_button = std::make_shared<Button>();
    collapse_button->set_text("");
    collapse_button->set_icon(expanded_tex);
    collapse_button->set_minimum_size(Vec2F(title_bar_height));
    collapse_button->set_expand_icon(true);
    collapse_button->theme_normal.border_width = 0.0;
    collapse_button->theme_normal.bg_color = ColorU::transparent_black();

    title_container = std::make_shared<HStackContainer>();
    title_container->set_parent(this);
    title_container->add_child(collapse_button);
    title_container->add_child(title_label);
    title_container->add_child(close_button);
    title_container->set_position({0, 0});
    title_container->set_size({size.x, title_bar_height});
    title_container->set_mouse_filter(MouseFilter::Ignore);

    // Connect signals.
    auto callback = [this] {
        collapsed = !collapsed;
        if (collapsed) {
            collapse_button->set_icon(collapsed_tex);

            // Store width.
            expanded_width = size.x;

            float new_width = title_container->calc_minimum_size().x;

            set_size({new_width, size.y});

            Logger::verbose("Collapsed", "Panel");
        } else {
            collapse_button->set_icon(expanded_tex);

            set_size({expanded_width, size.y});

            Logger::verbose("Expanded", "Panel");
        }
    };
    collapse_button->connect_signal("pressed", callback);

    close_button->connect_signal("pressed", [this] { visible = false; });

    title_bar = true;
    // ---------------------------------------------------------
}

void Panel::propagate_input(InputEvent &event) {
    if (title_bar) {
        title_container->propagate_input(event);
    }

    if (collapsed) {
        input(event);
    } else {
        Node::propagate_input(event);
    }
}

void Panel::input(InputEvent &event) {
    auto global_position = get_global_position();

    bool consume_flag = false;

    if (event.type == InputEventType::MouseMotion) {
        auto args = event.args.mouse_motion;

        if (title_bar_pressed) {
            set_position(title_bar_pressed_position + args.position - title_bar_pressed_mouse_position);
            consume_flag = true;
        }

        RectF active_rect;
        active_rect.left = global_position.x;
        active_rect.top = 0;
        active_rect.right = size.x;
        active_rect.bottom = collapsed ? title_bar_height : size.y + title_bar_height;

        if (active_rect.contains_point(args.position)) {
            consume_flag = true;
        }

        bool h_resize = false;
        if (abs(global_position.x - args.position.x) < 4.0 || abs(global_position.x + size.x - args.position.x) < 4.0) {
            h_resize = true;
            consume_flag = true;
        }

        bool v_resize = false;
        if (abs(global_position.y - args.position.y) < 4.0 || abs(global_position.y + size.y + title_bar_height - args.position.y) < 4.0) {
            v_resize = true;
            consume_flag = true;
        }

        if (h_resize && v_resize) {
            InputServer::get_singleton()->set_cursor(CursorShape::ResizeTlbr);
        } else if (h_resize) {
            InputServer::get_singleton()->set_cursor(CursorShape::ResizeH);
        } else if (v_resize) {
            InputServer::get_singleton()->set_cursor(CursorShape::ResizeV);
        } else {
            InputServer::get_singleton()->set_cursor(CursorShape::Arrow);
        }
    }

    if (event.type == InputEventType::MouseButton) {
        auto args = event.args.mouse_button;

        if (!event.is_consumed()) {
            if (RectF(global_position, global_position + Vec2<float>(size.x, title_bar_height))
                    .contains_point(args.position)) {
                title_bar_pressed = args.pressed;

                if (title_bar_pressed) {
                    title_bar_pressed_mouse_position = args.position;
                    title_bar_pressed_position = position;
                }

                consume_flag = true;
            }
        }

        if (!args.pressed) title_bar_pressed = false;
    }

    if (consume_flag) {
        event.consume();
    }

    if (!collapsed) {
        NodeUi::input(event);
    }
}

void Panel::update(double dt) {
    if (title_bar) {
        title_container->propagate_update(dt);
    }

    NodeUi::update(dt);
}

void Panel::propagate_draw(VkCommandBuffer cmd_buffer) {
    if (!visible) {
        return;
    }

    draw();

    if (!collapsed) {
        Node::propagate_draw(cmd_buffer);
    }
}

void Panel::draw() {
    auto vector_server = VectorServer::get_singleton();

    auto global_position = get_global_position();

    if (theme_panel.has_value()) {
        if (title_bar) {
            if (collapsed) {
                // Only draw title bar.
                vector_server->draw_style_box(
                    theme_panel.value(), get_global_position(), Vec2<float>(size.x, title_bar_height));
            } else {
                // Draw title bar + panel.
                vector_server->draw_style_box(
                    theme_panel.value(), get_global_position(), size + Vec2<float>(0, title_bar_height));

                vector_server->draw_style_line(theme_title_bar_line.value(),
                                               {global_position.x, global_position.y + title_bar_height},
                                               {global_position.x + size.x, global_position.y + title_bar_height});
            }

            title_container->propagate_draw(VK_NULL_HANDLE);
        } else {
            if (!collapsed) {
                vector_server->draw_style_box(theme_panel.value(), get_global_position(), size);
            }
        }
    }

    if (title_bar) {
        title_container->propagate_draw(VK_NULL_HANDLE);
    }
}

void Panel::enable_title_bar(bool enabled) {
    title_bar = enabled;
}

void Panel::set_size(Vec2<float> p_size) {
    if (size == p_size) {
        return;
    }
    size = p_size;
    title_container->set_size({size.x, title_bar_height});
}

void Panel::set_title(const std::string &title) {
    title_label->set_text(title);
}

} // namespace Flint
