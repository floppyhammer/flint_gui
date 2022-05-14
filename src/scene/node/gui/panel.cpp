#include "panel.h"

#include <string>

namespace Flint {
    Panel::Panel() {
        type = NodeType::Panel;

        theme_panel = std::make_optional(StyleBox());
        theme_panel.value().bg_color = ColorU(27, 27, 27, 255);
        //theme_panel.value().shadow_size = 8;
        //theme_panel.value().shadow_color = ColorU::black();
        theme_panel.value().border_width = 2;
        theme_panel.value().corner_radius = 8;
        theme_panel.value().border_color = {50, 50, 50, 255};

        title_label = std::make_shared<Label>();
        title_label->set_text("Title");
        title_label->set_horizontal_alignment(Alignment::Center);
        title_label->set_vertical_alignment(Alignment::Center);
        title_label->set_mouse_filter(MouseFilter::IGNORE);
        title_label->sizing_flag = ContainerSizingFlag::EXPAND;

        {
            auto close_icon = VectorTexture::from_empty(24, 24);
            SvgShape svg_shape;
            svg_shape.shape.add_line({-8, -8}, {8, 8});
            svg_shape.shape.add_line({-8, 8}, {8, -8});
            svg_shape.shape.translate({close_icon->get_width() * 0.5f, close_icon->get_height() * 0.5f});
            svg_shape.stroke_color = ColorU(163, 163, 163, 255);
            svg_shape.stroke_width = 2;
            close_icon->add_svg_shape(svg_shape);

            close_button = std::make_shared<Button>();
            close_button->set_text("");
            close_button->set_parent(this);
            close_button->set_icon(close_icon);
            close_button->set_minimum_size(Vec2<float>(title_bar_height));
        }

        {
            collapse_icon = VectorTexture::from_empty(24, 24);
            SvgShape svg_shape;
            svg_shape.shape.move_to(5, -5);
            svg_shape.shape.line_to(0, 7);
            svg_shape.shape.line_to(-5, -5);
            svg_shape.shape.close();
            svg_shape.shape.translate({collapse_icon->get_width() * 0.5f, collapse_icon->get_height() * 0.5f});
            svg_shape.stroke_color = ColorU(163, 163, 163, 255);
            svg_shape.stroke_width = 2;
            collapse_icon->add_svg_shape(svg_shape);
        }

        {
            expand_icon = VectorTexture::from_empty(24, 24);
            SvgShape svg_shape;
            svg_shape.shape.move_to(-5, -5);
            svg_shape.shape.line_to(7, 0);
            svg_shape.shape.line_to(-5, 5);
            svg_shape.shape.close();
            svg_shape.shape.translate({expand_icon->get_width() * 0.5f, expand_icon->get_height() * 0.5f});
            svg_shape.stroke_color = ColorU(163, 163, 163, 255);
            svg_shape.stroke_width = 2;
            expand_icon->add_svg_shape(svg_shape);
        }

        collapse_button = std::make_shared<Button>();
        collapse_button->set_text("");
        collapse_button->set_icon(collapse_icon);
        auto callback = [this] {
            collapsed = !collapsed;
            if (collapsed) {
                collapse_button->set_icon(expand_icon);
            } else {
                collapse_button->set_icon(collapse_icon);
            }
        };
        collapse_button->connect_signal("on_pressed", callback);
        collapse_button->set_minimum_size(Vec2<float>(title_bar_height));

        container = std::make_shared<HBoxContainer>();
        container->set_parent(this);
        container->add_child(collapse_button);
        container->add_child(title_label);
        container->add_child(close_button);
        container->set_position({0, -title_bar_height});
        container->set_size({size.x, title_bar_height});

        title_bar = true;
    }

    void Panel::propagate_input(std::vector<InputEvent> &input_queue) {
        if (!collapsed) {
            Node::propagate_input(input_queue);
        }

        container->propagate_input(input_queue);

        input(input_queue);
    }

    void Panel::input(std::vector<InputEvent> &input_queue) {
        auto global_position = get_global_position();

        collapse_button->input(input_queue);
        close_button->input(input_queue);

        for (auto &event: input_queue) {
            if (event.type == InputEventType::MouseMotion) {
                auto args = event.args.mouse_motion;

                if (title_bar_pressed) {
                    set_position(title_bar_pressed_position + args.position - title_bar_pressed_mouse_position);
                    event.consume();
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

                        event.consume();
                    }
                }

                if (!args.pressed) title_bar_pressed = false;
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
        auto canvas = VectorServer::get_singleton().canvas;

        auto global_position = get_global_position();

        if (theme_panel.has_value()) {
            if (title_bar) {
                if (!collapsed) {
                    theme_panel.value().add_to_canvas(get_global_position() - Vec2<float>(0, title_bar_height),
                                                      size + Vec2<float>(0, title_bar_height), canvas);

                    Pathfinder::Shape shape;
                    shape.add_line({}, {size.x, 0});
                    canvas->set_stroke_paint(Pathfinder::Paint::from_color({50, 50, 50, 255}));
                    canvas->set_line_width(2);
                    canvas->set_transform(Pathfinder::Transform2::from_translation({global_position.x, global_position.y}));
                    canvas->stroke_shape(shape);
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
