#ifndef FLINT_RESOURCE_STYLE_BOX_H
#define FLINT_RESOURCE_STYLE_BOX_H

#include <pathfinder.h>

#include "../common/geometry.h"

using Pathfinder::ColorU;
using Pathfinder::Vec2F;

namespace Flint {

struct StyleBox {
    ColorU bg_color = ColorU(27, 27, 27, 255);

    ColorU border_color = ColorU(67, 67, 67, 255);
    float border_width = 0;
    float corner_radius = 4;

    ColorU shadow_color;
    float shadow_size = 0;
    Vec2F shadow_offset;
};

struct StyleLine {
    ColorU color = ColorU(163, 163, 163, 255);

    float width = 2;
};

} // namespace Flint

#endif // FLINT_RESOURCE_STYLE_BOX_H
