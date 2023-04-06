#include "render.h"

#include <entt/entt.hpp>

#include "servers/render_server.h"

namespace Flint::Ecs {

void render_system(entt::registry &registry) {
    // Update uniform buffers (excluding those unchanged).
    entt::observer observer{registry, entt::collector.update<C_GlobalTransform>()};

    for (const auto entity : observer) {
        // ...
    }

    observer.clear();

    // Prepare render graph.
    //    std::vector<entt::organizer::vertex> render_graph = render_organizer.graph();
    //
    //    for (auto &&node : render_graph) {
    //        node.prepare(registry);
    //    }

    // Get render context.
    auto render_server = registry.ctx().get<RenderServer *>();

    // Render each renderable (a view is needed instead of an observer).
    auto view = registry.view<const C_GlobalTransform, C_Handle<Mesh>, C_Handle<Material>, C_Visibility>();

    //    // use a callback
    //    view.each([](const auto &pos, auto &vel) { /* ... */ });
    //
    //    // use an extended callback
    //    view.each([](const auto entity, const auto &pos, auto &vel) { /* ... */ });
    //
    //    // use forward iterators and get only the components of interest
    //    for (auto entity : view) {
    //        auto &vel = view.get<C_Transform>(entity);
    //        // ...
    //    }

    for (auto [entity, xform, mesh_handle, material_handle, visibility] : view.each()) {
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
