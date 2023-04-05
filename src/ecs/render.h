#ifndef FLINT_ECS_RENDER_H
#define FLINT_ECS_RENDER_H

#include "../common/mat3x3.h"
#include "../common/quat.h"

using namespace Flint::Math;

namespace Flint::Ecs {

struct C_Transform {
    Pathfinder::Vec3F translation;
    Quat rotation;
    Pathfinder::Vec3F scale;
};

struct C_GlobalTransform {
    Mat3x3<float> matrix3;
    Pathfinder::Vec3F translation;
};

struct C_Mesh {
    float x;
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_RENDER_H
