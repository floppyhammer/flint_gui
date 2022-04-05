#include "rigid_body_2d.h"

#include "../../../rendering/rendering_server.h"

namespace Flint {
    RigidBody2d::RigidBody2d() {
        type = NodeType::RigidBody2D;
    }

    void RigidBody2d::_update(double delta) {
        // Update self.
        update(delta);

        // Update children.
        Node::_update(delta);
    }

    void RigidBody2d::update(double delta) {
        position += velocity * delta;
        velocity += gravity * delta;

        if (position.x < 0 || position.x > WIDTH)
            velocity.x *= -1.0f;
        if (position.y < 0 || position.y > HEIGHT)
            velocity.y *= -1.0f;
    }
}
