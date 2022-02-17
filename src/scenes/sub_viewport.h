#ifndef FLINT_SUB_VIEWPORT_H
#define FLINT_SUB_VIEWPORT_H

#include "../common/vec2.h"

namespace Flint {
    class SubViewport {
    public:
        Vec2<int> size = {512, 512};

        float fov = 45.0;

        float z_near = 0.1;
        float z_far = 10.0;
    };
}

#endif //FLINT_SUB_VIEWPORT_H
