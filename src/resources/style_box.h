#ifndef FLINT_RESOURCE_STYLE_BOX_H
#define FLINT_RESOURCE_STYLE_BOX_H

#include "../common/geometry.h"

namespace Flint {

struct StyleBox {
    ColorU bg_color = ColorU(27, 27, 27, 255);

    ColorU border_color = ColorU(67, 67, 67, 255);
    float border_width = 0;
    float corner_radius = 4;

    ColorU shadow_color;
    float shadow_size = 0;
    Vec2F shadow_offset;

    static StyleBox from_empty() {
        StyleBox box;
        box.bg_color = ColorU::transparent_black();

        box.border_color = ColorU::transparent_black();
        box.border_width = 0;
        box.corner_radius = 0;

        box.shadow_color = ColorU::transparent_black();
        box.shadow_size = 0;
        box.shadow_offset = Vec2F();

        return box;
    }
};

struct StyleLine {
    ColorU color = ColorU(163, 163, 163, 255);

    float width = 2;
};

} // namespace Flint

#endif // FLINT_RESOURCE_STYLE_BOX_H
