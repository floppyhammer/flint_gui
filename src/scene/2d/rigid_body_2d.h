#ifndef FLINT_RIGID_BODY_2D_H
#define FLINT_RIGID_BODY_2D_H

#include "node_2d.h"

namespace Flint {
class RigidBody2d : public Node2d {
public:
    RigidBody2d();

    Vec2F velocity{0};
    Vec2F acceleration{0};
    Vec2F gravity{0};

protected:
    void update(double dt) override;
};
} // namespace Flint

#endif // FLINT_RIGID_BODY_2D_H
