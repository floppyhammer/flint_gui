#include "panel.h"

#include <string>

namespace Flint {
    Panel::Panel() {
        type = NodeType::Panel;

        theme_panel = std::make_optional(StyleBox());
        theme_panel.value().bg_color = ColorU(27, 27, 27, 255);
        theme_panel.value().border_color = {50, 50, 50, 255};
        theme_panel.value().border_width = 2;
        theme_panel.value().corner_radius = 8;

        theme_title_bar_line = std::make_optional(StyleLine());
        theme_title_bar_line.value().color = {50, 50, 50, 255};

        {
            collapse_icon = VectorTexture::from_empty(24, 24);
            VShape vshape;
            vshape.shape.move_to(6, -6);
            vshape.shape.line_to(0, 7);
            vshape.shape.line_to(-6, -6);
            vshape.shape.close();
            vshape.shape.translate({collapse_icon->get_width() * 0.5f, collapse_icon->get_height() * 0.5f});
            vshape.stroke_color = ColorU(163, 163, 163, 255);
            vshape.stroke_width = 2;
            collapse_icon->set_vshapes({vshape});
        }

        {
            expand_icon = VectorTexture::from_empty(24, 24);
            VShape vshape;
            vshape.shape.move_to(-6, -6);
            vshape.shape.line_to(6, 0);
            vshape.shape.line_to(-6, 6);
            vshape.shape.close();
            vshape.shape.translate({expand_icon->get_width() * 0.5f, expand_icon->get_height() * 0.5f});
            vshape.stroke_color = ColorU(163, 163, 163, 255);
            vshape.stroke_width = 2;
            expand_icon->set_vshapes({vshape});
        }

        {
            title_label = std::make_shared<Label>();
            title_label->set_text("Panel");
            title_label->set_horizontal_alignment(Alignment::Center);
            title_label->set_vertical_alignment(Alignment::Center);
            title_label->set_mouse_filter(MouseFilter::IGNORE);
            title_label->sizing_flag = ContainerSizingFlag::EXPAND;
        }

        {
            auto close_icon = VectorTexture::from_empty(24, 24);
            VShape vshape;
            vshape.shape.add_line({-8, -8}, {8, 8});
            vshape.shape.add_line({-8, 8}, {8, -8});
            vshape.shape.translate({close_icon->get_width() * 0.5f, close_icon->get_height() * 0.5f});
            vshape.stroke_color = ColorU(163, 163, 163, 255);
            vshape.stroke_width = 2;
            close_icon->set_vshapes({vshape});

            close_button = std::make_shared<Button>();
            close_button->set_text("");
            close_button->set_parent(this);
            close_button->set_icon(close_icon);
            close_button->set_minimum_size(Vec2<float>(title_bar_height));
        }

        collapse_button = std::make_shared<Button>();
        collapse_button->set_text("");
        collapse_button->set_icon(collapse_icon);
        collapse_button->set_minimum_size(Vec2<float>(title_bar_height));

        container = std::make_shared<BoxContainer>();
        container->set_parent(this);
        container->add_child(collapse_button);
        container->add_child(title_label);
        container->add_child(close_button);
        container->set_position({0, -title_bar_height});
        container->set_size({size.x, title_bar_height});
        container->set_mouse_filter(MouseFilter::IGNORE);

        // Connect signals.
        auto callback = [this] {
            collapsed = !collapsed;
            if (collapsed) {
                collapse_button->set_icon(expand_icon);

                // Store width.
                expanded_width = size.x;

                float new_width = container->calculate_minimum_size().x;

                container->set_size({new_width, title_bar_height});

                size.x = new_width;

                Logger::verbose("Collapsed", "Panel");
            } else {
                collapse_button->set_icon(collapse_icon);

                container->set_size({expanded_width, title_bar_height});
                size.x = expanded_width;

                Logger::verbose("Expanded", "Panel");
            }
        };
        collapse_button->connect_signal("on_pressed", callback);

        title_bar = true;
    }

    void Panel::propagate_input(std::vector<InputEvent> &input_queue) {
        container->propagate_input(input_queue);

        if (collapsed) {
            input(input_queue);
        } else {
            Node::propagate_input(input_queue);
        }
    }

    void Panel::input(std::vector<InputEvent> &input_queue) {
        auto global_position = get_global_position();

        for (auto &event: input_queue) {
            bool consume_flag = false;

            if (event.type == InputEventType::MouseMotion) {
                auto args = event.args.mouse_motion;

                if (title_bar_pressed) {
                    set_position(title_bar_pressed_position + args.position - title_bar_pressed_mouse_position);
                    consume_flag = true;
                }

                Rect<float> active_rect;
                active_rect.left = global_position.x;
                active_rect.top = -title_bar_height;
                active_rect.right = size.x;
                active_rect.bottom = collapsed ? title_bar_height : size.y + title_bar_height;

                if (active_rect.contains_point(args.position)) {
                    consume_flag = true;
                }
            }

            if (event.type == InputEventType::MouseButton) {
                auto args = event.args.mouse_button;

                if (!event.is_consumed()) {
                    if (Rect<float>(global_position - Vec2<float>(0, title_bar_height),
                                    global_position + Vec2<float>(size.x, 0)).contains_point(args.position)) {
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
        }

        if (!collapsed) {
            Control::input(input_queue);
        }
    }

    void Panel::update(double dt) {
        container->propagate_update(dt);

        Control::update(dt);
    }

    void Panel::propagate_draw(VkCommandBuffer p_command_buffer) {
        draw(p_command_buffer);

        if (!collapsed) {
            Node::propagate_draw(p_command_buffer);
        }
    }

    void Panel::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton()->canvas;

        auto global_position = get_global_position();

        if (theme_panel.has_value()) {
            if (title_bar) {
                if (!collapsed) {
                    theme_panel.value().add_to_canvas(get_global_position() - Vec2<float>(0, title_bar_height),
                                                      size + Vec2<float>(0, title_bar_height), canvas);

                    theme_title_bar_line->add_to_canvas({global_position.x, global_position.y}, {global_position.x + size.x, global_position.y}, canvas);
                } else {
                    theme_panel.value().add_to_canvas(get_global_position() - Vec2<float>(0, title_bar_height),
                                                      Vec2<float>(size.x, title_bar_height), canvas);
                }

                container->propagate_draw(p_command_buffer);
            } else {
                if (!collapsed) {
                    theme_panel.value().add_to_canvas(get_global_position(), size, canvas);
                }
            }
        }

        if (title_bar) {
            container->propagate_draw(p_command_buffer);
        }
    }

    void Panel::enable_title_bar(bool enabled) {
        title_bar = enabled;
    }

    void Panel::set_size(Vec2<float> p_size) {
        if (size == p_size) return;
        size = p_size;
        container->set_size({size.x, title_bar_height});
    }
}
