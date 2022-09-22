#ifndef FLINT_ECS_WORLD_H
#define FLINT_ECS_WORLD_H

#include <memory>
#include <vector>

#include "coordinator.h"
#include "entity.h"
#include "systems/hierarchy_system.h"
#include "systems/model_render_system.h"
#include "systems/physics_system.h"
#include "systems/sprite_render_system.h"

namespace Flint {
class World {
public:
    World();

    ~World();

    Entity spawn() {
        auto entity = coordinator->create_entity();

        entities.insert(entity);

        return entity;
    }

    template <typename T>
    void add_component(Entity entity, T component) {
        coordinator->add_component(entity, component);
    }

    void update(double dt);

    void draw(VkCommandBuffer command_buffer);

private:
    std::shared_ptr<Coordinator> coordinator;

    std::set<Flint::Entity> entities;

    std::shared_ptr<Flint::Sprite2dRenderSystem> sprite_render_system;

    std::shared_ptr<Flint::ModelRenderSystem> model_render_system;

    std::shared_ptr<Flint::Physics2dSystem> physics_system;

    std::shared_ptr<Flint::HierarchySystem> hierarchy_system;
};
} // namespace Flint

#endif // FLINT_ECS_WORLD_H
