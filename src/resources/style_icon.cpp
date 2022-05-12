#include "style_icon.h"

namespace Flint {
    void StyleIcon::add_to_canvas(const Vec2<float> &position,
                                   const std::shared_ptr<Pathfinder::Canvas>& canvas) {
        auto transform = Pathfinder::Transform2::from_translation({position.x, position.y});
        canvas->set_transform(transform);
        canvas->set_line_width(2);
        canvas->set_stroke_paint(Pathfinder::Paint::from_color({color.r, color.g, color.b, color.a}));
        canvas->stroke_shape(shape);
    }
}
