#ifndef FLINT_RESOURCE_STYLE_BOX_H
#define FLINT_RESOURCE_STYLE_BOX_H

#include <pathfinder.h>

#include "../common/geometry.h"

using Pathfinder::Vec2;
using Pathfinder::ColorU;

namespace Flint {
    struct StyleBox {
        ColorU bg_color = ColorU(27, 27, 27, 255);

        ColorU border_color = ColorU(67, 67, 67, 255);
        float border_width = 0;
        float corner_radius = 4;

        float margin = 0;

        ColorU shadow_color;
        float shadow_size;
        Vec2<float> shadow_offset;

        void add_to_canvas(const Vec2<float> &position,
                           const Vec2<float> &size,
                           const std::shared_ptr<Pathfinder::Canvas> &canvas);
    };

    struct StyleLine {
        ColorU color = ColorU(163, 163, 163, 255);

        float width = 2;

        void add_to_canvas(const Vec2<float> &start,
                           const Vec2<float> &end,
                           const std::shared_ptr<Pathfinder::Canvas> &canvas);
    };
}

#endif //FLINT_RESOURCE_STYLE_BOX_H
