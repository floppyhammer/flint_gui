#include "panel.h"

#include <string>

#include "../../common/logger.h"

namespace Flint {

Panel::Panel() {
    type = NodeType::Panel;

    theme_panel = std::make_optional(StyleBox());
    theme_panel.value().bg_color = ColorU(27, 27, 27, 255);
    theme_panel.value().border_color = {50, 50, 50, 100};
    theme_panel.value().border_width = 2;
    theme_panel.value().corner_radius = 8;

    // Set up title bar.
    // ---------------------------------------------------------
    theme_title_bar_line = std::make_optional(StyleLine());
    theme_title_bar_line.value().color = {50, 50, 50, 255};

    {
        collapse_icon = VectorTexture::from_empty(24, 24);
        VectorPath vp;
        float x = collapse_icon->get_width() * 0.5f;
        float y = collapse_icon->get_height() * 0.5f;
        vp.path2d.move_to(6 + x, -6 + y);
        vp.path2d.line_to(0 + x, 7 + y);
        vp.path2d.line_to(-6 + x, -6 + y);
        vp.path2d.close_path();
        vp.stroke_color = ColorU(163, 163, 163, 255);
        vp.stroke_width = 2;
        collapse_icon->add_path(vp);
    }

    {
        expand_icon = VectorTexture::from_empty(24, 24);
        VectorPath vp;
        float x = expand_icon->get_width() * 0.5f;
        float y = expand_icon->get_height() * 0.5f;
        vp.path2d.move_to(-6 + x, -6 + y);
        vp.path2d.line_to(6 + x, 0 + y);
        vp.path2d.line_to(-6 + x, 6 + y);
        vp.path2d.close_path();

        vp.stroke_color = ColorU(163, 163, 163, 255);
        vp.stroke_width = 2;
        expand_icon->add_path(vp);
    }

    {
        title_label = std::make_shared<Label>("Panel");
        title_label->set_horizontal_alignment(Alignment::Center);
        title_label->set_vertical_alignment(Alignment::Center);
        title_label->set_mouse_filter(MouseFilter::Ignore);
        title_label->container_sizing.expand_h = true;
        title_label->container_sizing.flag_h = ContainerSizingFlag::Fill;
    }

    {
        auto close_icon = VectorTexture::from_empty(24, 24);
        VectorPath vp;
        float x = close_icon->get_width() * 0.5f;
        float y = close_icon->get_height() * 0.5f;
        vp.path2d.add_line({-8 + x, -8 + y}, {8 + x, 8 + y});
        vp.path2d.add_line({-8 + x, 8 + y}, {8 + x, -8 + y});

        vp.stroke_color = ColorU(163, 163, 163, 255);
        vp.stroke_width = 2;
        close_icon->add_path(vp);

        close_button = std::make_shared<Button>();
        close_button->set_text("");
        close_button->set_parent(this);
        close_button->set_icon(close_icon);
        close_button->set_minimum_size(Vec2F(title_bar_height));
        close_button->set_expand_icon(true);
    }

    collapse_button = std::make_shared<Button>();
    collapse_button->set_text("");
    collapse_button->set_icon(collapse_icon);
    collapse_button->set_minimum_size(Vec2F(title_bar_height));
    collapse_button->set_expand_icon(true);

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
            collapse_button->set_icon(expand_icon);

            // Store width.
            expanded_width = size.x;

            float new_width = title_container->calc_minimum_size().x;

            set_size({new_width, size.y});

            Logger::verbose("Collapsed", "Panel");
        } else {
            collapse_button->set_icon(collapse_icon);

            set_size({expanded_width, size.y});

            Logger::verbose("Expanded", "Panel");
        }
    };
    collapse_button->connect_signal("pressed", callback);

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
        Control::input(event);
    }
}

void Panel::update(double dt) {
    if (title_bar) {
        title_container->propagate_update(dt);
    }

    Control::update(dt);
}

void Panel::propagate_draw(VkCommandBuffer p_command_buffer) {
    draw(p_command_buffer);

    if (!collapsed) {
        Node::propagate_draw(p_command_buffer);
    }
}

void Panel::draw(VkCommandBuffer p_command_buffer) {
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

            title_container->propagate_draw(p_command_buffer);
        } else {
            if (!collapsed) {
                vector_server->draw_style_box(theme_panel.value(), get_global_position(), size);
            }
        }
    }

    if (title_bar) {
        title_container->propagate_draw(p_command_buffer);
    }
}

void Panel::enable_title_bar(bool enabled) {
    title_bar = enabled;
}

void Panel::set_size(Vec2<float> p_size) {
    if (size == p_size) return;
    size = p_size;
    title_container->set_size({size.x, title_bar_height});
}

void Panel::set_title(const std::string& title) {
    title_label->set_text(title);
}

} // namespace Flint
