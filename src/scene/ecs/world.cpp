#include "world.h"

#include "components/components.h"

namespace Flint {
    World::World() {
        coordinator = std::make_unique<Coordinator>();

        // Register components.
        coordinator->register_component<Flint::GravityComponent>();
        coordinator->register_component<Flint::RigidBodyComponent>();
        coordinator->register_component<Flint::Transform2dComponent>();
        coordinator->register_component<Flint::Transform3dComponent>();
        coordinator->register_component<Flint::TransformGuiComponent>();
        coordinator->register_component<Flint::Sprite2dComponent>();
        coordinator->register_component<Flint::Sprite3dComponent>();
        coordinator->register_component<Flint::ModelComponent>();
        coordinator->register_component<Flint::ViewportInputComponent>();
        coordinator->register_component<Flint::ViewportOutputComponent>();
        coordinator->register_component<Flint::ZSort2d>();
        coordinator->register_component<Flint::HierarchicalRelations>();

        // Register systems.
        physics_system = coordinator->register_system<Flint::Physics2dSystem>();
        // Set signature.
        {
            Flint::Signature signature;
            signature.set(coordinator->get_component_type<Flint::GravityComponent>());
            signature.set(coordinator->get_component_type<Flint::RigidBodyComponent>());
            signature.set(coordinator->get_component_type<Flint::Transform2dComponent>());
            coordinator->set_system_signature<Flint::Physics2dSystem>(signature);
        }

        sprite_render_system = coordinator->register_system<Flint::Sprite2dRenderSystem>();
        // Set signature.
        {
            Flint::Signature signature;
            signature.set(coordinator->get_component_type<Flint::Sprite2dComponent>());
            signature.set(coordinator->get_component_type<Flint::Transform2dComponent>());
            signature.set(coordinator->get_component_type<Flint::ZSort2d>());
            coordinator->set_system_signature<Flint::Sprite2dRenderSystem>(signature);
        }

        model_render_system = coordinator->register_system<Flint::ModelRenderSystem>();
        // Set signature.
        {
            Flint::Signature signature;
            signature.set(coordinator->get_component_type<Flint::ModelComponent>());
            signature.set(coordinator->get_component_type<Flint::Transform3dComponent>());
            coordinator->set_system_signature<Flint::ModelRenderSystem>(signature);
        }

        hierarchy_system = coordinator->register_system<Flint::HierarchySystem>();
        // Set signature.
        {
            Flint::Signature signature;
            signature.set(coordinator->get_component_type<Flint::HierarchicalRelations>());
            coordinator->set_system_signature<Flint::HierarchySystem>(signature);
        }
    }

    World::~World() {
        for (auto &entity: entities) {
            coordinator->destroy_entity(entity);
        }
        entities.clear();
    }

    void World::update(double dt) {
        physics_system->update(coordinator, dt);
        sprite_render_system->update(coordinator, dt);
        model_render_system->update(coordinator);
    }

    void World::draw(VkCommandBuffer command_buffer) {
        sprite_render_system->draw(coordinator, command_buffer);
        model_render_system->draw(coordinator, command_buffer);
    }
}
