#ifndef FLINT_GEOMETRY_H
#define FLINT_GEOMETRY_H

#include <pathfinder.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Flint {

typedef Pathfinder::ColorU ColorU;
typedef Pathfinder::ColorF ColorF;
typedef Pathfinder::RectF RectF;
typedef Pathfinder::RectI RectI;
typedef Pathfinder::Transform2 Transform2;
typedef Pathfinder::Vec2F Vec2F;
typedef Pathfinder::Vec2I Vec2I;

} // namespace Flint

#endif // FLINT_GEOMETRY_H
