#ifndef FLINT_RESOURCE_STYLE_BOX_H
#define FLINT_RESOURCE_STYLE_BOX_H

#include <pathfinder.h>

using Pathfinder::ColorU;
using Pathfinder::Vec2;

namespace Flint {
    struct StyleBox {
        ColorU bg_color = ColorU(32, 32, 32, 255);
        bool draw_center = true;

        ColorU border_color = ColorU(67, 67, 67, 255);
        float border_width = 2;

        float corner_radius = 8;

        float margin = 0;

        ColorU shadow_color;
        float shadow_size;
        Vec2<float> shadow_offset;
    };
}

#endif //FLINT_RESOURCE_STYLE_BOX_H
