#ifndef FLINT_STYLE_ICON_H
#define FLINT_STYLE_ICON_H

#include <pathfinder.h>

#include "../common/color.h"
#include "../common/math/vec2.h"

namespace Flint {
    struct StyleIcon {
        Pathfinder::Shape shape;
        ColorU color;

        void add_to_canvas(const Vec2<float> &position,
                           const std::shared_ptr<Pathfinder::Canvas> &canvas);
    };
}

#endif //FLINT_STYLE_ICON_H
