#include "style_box.h"

namespace Flint {
void StyleBox::add_to_canvas(const Vec2<float> &position,
                             const Vec2<float> &size,
                             const std::shared_ptr<Pathfinder::Canvas> &canvas) {
    canvas->save_state();

    // Rebuild & draw the style box.
    auto style_box_outline = Pathfinder::Outline();
    style_box_outline.add_rect({0, 0, size.x, size.y}, corner_radius);

    canvas->set_shadow_color({shadow_color.r, shadow_color.g, shadow_color.b, shadow_color.a});
    canvas->set_shadow_blur(shadow_size);

    auto transform = Pathfinder::Transform2::from_translation({position.x, position.y});
    canvas->set_transform(transform);
    canvas->set_fill_paint(Pathfinder::Paint::from_color({bg_color.r, bg_color.g, bg_color.b, bg_color.a}));
    canvas->fill_path(style_box_outline, Pathfinder::FillRule::Winding);

    if (border_width > 0) {
        canvas->set_stroke_paint(
            Pathfinder::Paint::from_color({border_color.r, border_color.g, border_color.b, border_color.a}));
        canvas->set_line_width(border_width);
        canvas->stroke_path(style_box_outline);
    }

    canvas->restore_state();
}

void StyleLine::add_to_canvas(const Vec2<float> &start,
                              const Vec2<float> &end,
                              const std::shared_ptr<Pathfinder::Canvas> &canvas) {
    canvas->save_state();

    auto outline = Pathfinder::Outline();
    outline.add_line({start.x, start.y}, {end.x, end.y});

    auto transform = Pathfinder::Transform2::from_translation({0, 0});
    canvas->set_transform(transform);
    canvas->set_stroke_paint(Pathfinder::Paint::from_color({color.r, color.g, color.b, color.a}));
    canvas->set_line_width(width);
    canvas->stroke_path(outline);

    canvas->restore_state();
}
} // namespace Flint
