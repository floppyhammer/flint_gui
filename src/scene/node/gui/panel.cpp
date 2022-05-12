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
        title_label->set_parent(this);
        title_label->set_mouse_filter(MouseFilter::IGNORE);

        StyleIcon close_icon;
        close_icon.shape.add_line({-8, -8}, {8, 8});
        close_icon.shape.add_line({-8, 8}, {8, -8});
        close_icon.shape.translate({close_icon.size.x * 0.5f, close_icon.size.y * 0.5f});
        close_button = std::make_shared<Button>();
        close_button->set_text("");
        close_button->set_parent(this);
        close_button->set_icon(close_icon);

        collapse_icon.shape.move_to(5, -5);
        collapse_icon.shape.line_to(0, 7);
        collapse_icon.shape.line_to(-5, -5);
        collapse_icon.shape.close();
        collapse_icon.shape.translate({collapse_icon.size.x * 0.5f, collapse_icon.size.y * 0.5f});

        expand_icon.shape.move_to(-5, -5);
        expand_icon.shape.line_to(7, 0);
        expand_icon.shape.line_to(-5, 5);
        expand_icon.shape.close();
        expand_icon.shape.translate({expand_icon.size.x * 0.5f, expand_icon.size.y * 0.5f});

        collapse_button = std::make_shared<Button>();
        collapse_button->set_text("");
        collapse_button->set_icon(collapse_icon);
        collapse_button->set_parent(this);
        auto callback = [this] {
            collapsed = !collapsed;
            if (collapsed) {
                collapse_button->set_icon(expand_icon);
            } else {
                collapse_button->set_icon(collapse_icon);
            }
        };
        collapse_button->connect_signal("on_pressed", callback);

        title_bar = true;
    }

    void Panel::propagate_input(std::vector<InputEvent> &input_queue) {
        if (!collapsed) {
            Node::propagate_input(input_queue);
        }

        input(input_queue);
    }

    void Panel::input(std::vector<InputEvent> &input_queue) {
        auto global_position = get_global_position();

        collapse_button->input(input_queue);
        close_button->input(input_queue);

        for (auto &event: input_queue) {
            if (event.type == InputEventType::MouseMotion) {
                auto args = event.args.mouse_motion;

                if (event.is_consumed()) {
                    title_bar_pressed = false;
                } else {
                    if (title_bar_pressed) {
                        set_position(args.relative + position);
                    }
                }
            }

            if (event.type == InputEventType::MouseButton) {
                auto args = event.args.mouse_button;

                if (!event.is_consumed()) {
                    if (Rect<float>(global_position - Vec2<float>(0, title_bar_height),
                                    global_position + Vec2<float>(size.x, 0)).contains_point(args.position)) {
                        title_bar_pressed = args.pressed;
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
        title_label->set_position({0, -title_bar_height});
        title_label->set_size({size.x, title_bar_height});
        title_label->update(dt);

        collapse_button->set_position({0, -title_bar_height});
        collapse_button->set_size({title_bar_height, title_bar_height});
        collapse_button->update(dt);

        close_button->set_position({size.x - close_button->get_size().x, -title_bar_height});
        close_button->set_size({title_bar_height, title_bar_height});
        close_button->update(dt);

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

                // Collapse button.
                collapse_button->draw(p_command_buffer);

                // Close button.
                close_button->draw(p_command_buffer);
            } else {
                if (!collapsed) {
                    theme_panel.value().add_to_canvas(get_global_position(), size, canvas);
                }
            }
        }

        if (title_bar) {
            title_label->draw(p_command_buffer);
        }
    }

    void Panel::enable_title_bar(bool enabled) {
        title_bar = enabled;
    }
}
