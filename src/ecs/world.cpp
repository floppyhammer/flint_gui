#include "world.h"

namespace Flint::Ecs {

World::World(RenderServer *render_server) {
    // Add resources.
    registry.ctx().insert_or_assign(render_server);
}

World::~World() {
    // Remove resources.
    registry.ctx().erase<RenderServer *>();

    // Clear all entities.
    registry.clear();
}

entt::entity World::spawn() {
    const auto entity = registry.create();

    return entity;
}

void World::update() {

}

} // namespace Flint::Ecs
