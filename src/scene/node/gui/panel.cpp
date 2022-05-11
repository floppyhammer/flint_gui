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

        title_bar = true;
    }

    void Panel::input(std::vector<InputEvent> &input_queue) {
        auto global_position = get_global_position();

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
                    if (Rect<float>(global_position - Vec2<float>(0, title_bar_height), global_position + Vec2<float>(size.x, 0)).contains_point(args.position)) {
                        title_bar_pressed = args.pressed;
                        event.consume();
                    }
                }

                if (!args.pressed) title_bar_pressed = false;
            }
        }

        Control::input(input_queue);
    }

    void Panel::update(double dt) {
        title_label->set_position({0, -title_bar_height});
        title_label->set_size({size.x, title_bar_height});
        title_label->update(dt);

        Control::update(dt);
    }

    void Panel::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton().canvas;

        auto global_position = get_global_position();

        if (theme_panel.has_value()) {
            if (title_bar) {
                theme_panel.value().add_to_canvas(get_global_position() - Vec2<float>(0, title_bar_height), size + Vec2<float>(0, title_bar_height), canvas);
                Pathfinder::Shape shape;
                shape.add_line({}, {size.x, 0});
                canvas->set_stroke_paint(Pathfinder::Paint::from_color({50, 50, 50, 255}));
                canvas->set_line_width(2);
                canvas->set_transform(Pathfinder::Transform2::from_translation({global_position.x, global_position.y}));
                canvas->stroke_shape(shape);

                // Close button.
                Pathfinder::Shape shape_close;
                shape_close.add_line({-8, -8}, {8, 8});
                shape_close.add_line({-8, 8}, {8, -8});
                canvas->set_shadow_blur(0);
                canvas->set_stroke_paint(Pathfinder::Paint::from_color({163, 163, 163, 255}));
                canvas->set_line_width(2);
                canvas->set_transform(Pathfinder::Transform2::from_translation({global_position.x + size.x - title_bar_height * 0.5f, global_position.y - title_bar_height * 0.5f}));
                canvas->stroke_shape(shape_close);
            } else {
                theme_panel.value().add_to_canvas(get_global_position(), size, canvas);
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
