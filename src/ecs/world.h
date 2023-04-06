#ifndef FLINT_ECS_WORLD_H
#define FLINT_ECS_WORLD_H

#include <entt/entt.hpp>
#include <memory>
#include <vector>

#include "../servers/render_server.h"

namespace Flint::Ecs {

class World {
public:
    explicit World(RenderServer *render_server);

    ~World();

    entt::entity spawn();

    template <typename Type, typename... Args>
    void emplace_component(const entt::entity entity, Args &&...args) {
        registry.emplace<Type>(entity, std::forward<Args>(args)...);
    }

    template <typename T>
    void add_component(const entt::entity entity, T component) {
        auto &c = registry.emplace<T>(entity);
        c = component;
    }

    // Advances the execution of the Schedule by one cycle.
    void update();

    void add_system() {
    }

private:
    entt::registry registry;

    entt::organizer physics_organizer;
    entt::organizer render_organizer;
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_WORLD_H
