#pragma once

#include <optional>

#include "../common/geometry.h"

namespace revector {

struct StyleBox {
    ColorU bg_color = ColorU(27, 27, 27, 255);

    ColorU border_color = ColorU(67, 67, 67, 255);
    float border_width = 0;
    float corner_radius = 8;

    // Top-left, top-eight, bottom-left, bottom-right.
    std::optional<RectF> corner_radii;

    ColorU shadow_color;
    float shadow_size = 0;
    Vec2F shadow_offset;

    static StyleBox from_empty() {
        StyleBox box;
        box.bg_color = ColorU::transparent_black();

        box.border_color = ColorU::transparent_black();
        box.border_width = 0;
        box.corner_radius = {};

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

} // namespace revector
