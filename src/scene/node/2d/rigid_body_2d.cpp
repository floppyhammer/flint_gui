#include "rigid_body_2d.h"

#include "../../../render/render_server.h"

namespace Flint {
RigidBody2d::RigidBody2d() {
    type = NodeType::RigidBody2D;
}

void RigidBody2d::update(double dt) {
    position += velocity * dt;
    velocity += gravity * dt;

    if (position.x < 0) {
        velocity.x = std::abs(velocity.x);
    } else if (position.x > 400) {
        velocity.x = -std::abs(velocity.x);
    }
    if (position.y < 0) {
        velocity.y = std::abs(velocity.y);
    } else if (position.y > 400) {
        velocity.y = -std::abs(velocity.y);
    }
}
} // namespace Flint
