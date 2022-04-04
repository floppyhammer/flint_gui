#include "physics_system.h"

#include "../components/components.h"
#include "../coordinator.h"

namespace Flint {
    void Physics2dSystem::update(double dt) {
        for (auto const &entity: entities) {
            auto coordinator = Coordinator::get_singleton();

            auto &rigidBody = coordinator.get_component<RigidBody>(entity);
            auto &transform = coordinator.get_component<Transform2d>(entity);
            auto const &gravity = coordinator.get_component<Gravity>(entity);

            transform.position += rigidBody.velocity.xy() * dt;
            rigidBody.velocity += gravity.force * dt;

            if (transform.position.x < 0 || transform.position.x > WIDTH)
                rigidBody.velocity.x *= -1.0f;
            if (transform.position.y < 0 || transform.position.y > HEIGHT)
                rigidBody.velocity.y *= -1.0f;
        }
    }

    void Physics3dSystem::update(double dt) {
        for (auto const &entity: entities) {
            auto coordinator = Coordinator::get_singleton();

            auto &rigidBody = coordinator.get_component<RigidBody>(entity);
            auto &transform = coordinator.get_component<Transform3d>(entity);
            auto const &gravity = coordinator.get_component<Gravity>(entity);

            transform.position += rigidBody.velocity * dt;
            rigidBody.velocity += gravity.force * dt;
        }
    }
}
