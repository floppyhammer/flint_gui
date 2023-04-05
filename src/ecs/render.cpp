#include "render.h"

#include <entt/entt.hpp>

namespace Flint::Ecs {

void render(entt::registry &registry) {
    entt::observer observer{registry, entt::collector.update<C_Transform>()};
    for (const auto entity : observer) {
        // ...
    }

    observer.clear();

    auto view = registry.view<const C_Transform, C_Mesh>();

    // use a callback
    view.each([](const auto &pos, auto &vel) { /* ... */ });

    // use an extended callback
    view.each([](const auto entity, const auto &pos, auto &vel) { /* ... */ });

    // use a range-for
    for (auto [entity, pos, vel] : view.each()) {
        // ...
    }

    // use forward iterators and get only the components of interest
    for (auto entity : view) {
        auto &vel = view.get<C_Transform>(entity);
        // ...
    }
}

void reactive_update(entt::registry &registry) {
    // Observing matcher: an observer returns at least the entities for which
    // one or more of the given components have been updated and not yet destroyed.
    entt::observer observer{registry, entt::collector.update<C_Transform>()};

    for (const auto entity : observer) {
        // ...
    }

    observer.clear();
}

void y_sort(entt::registry &registry) {
    registry.sort<C_Transform>([](const auto &lhs, const auto &rhs) { return lhs.translation.z < rhs.translation.z; });
}

} // namespace Flint::Ecs
