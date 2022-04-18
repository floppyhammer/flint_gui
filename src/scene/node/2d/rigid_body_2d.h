#ifndef FLINT_RIGID_BODY_2D_H
#define FLINT_RIGID_BODY_2D_H

#include "node_2d.h"

namespace Flint {
    class RigidBody2d : public Node2d{
    public:
        RigidBody2d();

        Vec2<float> velocity{0};
        Vec2<float> acceleration{0};
        Vec2<float> gravity{0};

        void _update(double delta) override;

    protected:
        void update(double delta) override;
    };
}

#endif //FLINT_RIGID_BODY_2D_H
