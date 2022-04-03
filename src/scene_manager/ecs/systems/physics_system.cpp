#include "physics_system.h"

#include "../components/components.h"
#include "../coordinator.h"

namespace Flint {
    void PhysicsSystem::update(float dt) {
        for (auto const &entity: entities) {
            auto coordinator = Coordinator::get_singleton();

            auto &rigidBody = coordinator.get_component<RigidBody>(entity);
            auto &transform = coordinator.get_component<Transform3D>(entity);
            auto const &gravity = coordinator.get_component<Gravity>(entity);

            //transform.position += rigidBody.velocity * dt;
            //rigidBody.velocity += gravity.force * dt;
        }
    }
}
