#include "panel.h"

#include <string>

namespace Flint {
    Panel::Panel() {
        type = NodeType::Panel;

        theme_panel = std::make_optional(StyleBox());
        theme_panel.value().bg_color = ColorU(50, 50, 50, 255);
        theme_panel.value().shadow_size = 8;
        theme_panel.value().shadow_color = ColorU::black();

        title_label = std::make_shared<Label>();
        title_label->set_text("Title");
        title_label->set_horizontal_alignment(Alignment::Center);
        title_label->set_vertical_alignment(Alignment::Center);
        title_label->set_parent(this);

        title_bar = true;
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
                canvas->set_stroke_paint(Pathfinder::Paint::from_color(Pathfinder::ColorU::white()));
                canvas->set_line_width(2);
                canvas->set_transform(Pathfinder::Transform2::from_translation({global_position.x, global_position.y}));
                canvas->stroke_shape(shape);

                // Close button.
                Pathfinder::Shape shape_close;
                shape_close.add_line({-12, -12}, {12, 12});
                shape_close.add_line({-12, 12}, {12, -12});
                canvas->set_shadow_blur(0);
                canvas->set_stroke_paint(Pathfinder::Paint::from_color(Pathfinder::ColorU::white()));
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
