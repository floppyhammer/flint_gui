#include "mod.h"

#include "../world.h"

namespace Flint::Ecs {

Result<int, NodeRunError> MainPass2dNode::run(const RenderGraphContext& graph,
                                              RenderContext& render_context,
                                              const World& world) const {
    auto& view_entity = graph.view_entity.value();

    const auto [camera, transparent_phase, target, camera_2d] =
        world.registry.get<C_ExtractedCamera, C_RenderPhase<Transparent2d>, C_ViewTarget, C_Camera2d>(view_entity);

    return {0};
}

} // namespace Flint::Ecs
