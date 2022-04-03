#ifndef FLINT_COMPONENTS_H
#define FLINT_COMPONENTS_H

#include "../../../common/math/vec2.h"
#include "../../../common/math/vec3.h"
#include "../../../common/math/quaternion.h"

namespace Flint {
    struct RigidBody {
        Vec3<float> velocity;
        Vec3<float> acceleration;
    };

    struct Transform {
        Vec3<float> position;
        Quaternion rotation;
        Vec3<float> scale;
    };

    struct Transform2D {
        Vec2<float> position;
        float rotation;
        Vec2<float> scale;
    };

    struct Gravity {
        Vec3<float> force;
    };
}

#endif //FLINT_COMPONENTS_H
