#include "world.h"

namespace Flint::Ecs {

World::World(RenderServer *render_server) {
    registry.ctx().insert_or_assign(render_server);
}

World::~World() {
    // resets the context variable by type
    registry.ctx().erase<RenderServer *>();

    registry.clear();
}

entt::entity World::spawn() {
    const auto entity = registry.create();

    return entity;
}

void World::update(double dt) {
}

void World::render() {
    auto &var = registry.ctx().get<RenderServer *>();

    std::vector<entt::organizer::vertex> render_graph = render_organizer.graph();

    for (auto &&node : render_graph) {
        node.prepare(registry);
    }
}

} // namespace Flint::Ecs
