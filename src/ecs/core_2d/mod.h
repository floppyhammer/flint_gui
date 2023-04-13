#ifndef FLINT_RENDER_CORE_3D_H
#define FLINT_RENDER_CORE_3D_H

#include <entt/entt.hpp>

#include "../../common/geometry.h"
#include "../camera.h"
#include "../render_graph/context.h"
#include "../render_graph/node.h"
#include "../render_phase/phase_item.h"

namespace Flint::Ecs {

const char* CORE_2D = "core_2d";

class App;

class World;

struct C_ViewTarget {
    //    main_textures: MainTargetTextures,
    VkFormat main_texture_format;
    /// 0 represents `main_textures.a`, 1 represents `main_textures.b`
    /// This is shared across view targets with the same render target
    //                                          main_texture: Arc<AtomicUsize>,
    VkImageView out_texture;
    VkFormat out_texture_format;
};

class Transparent2d : public PhaseItem {
    // pub sort_key: FloatOrd,

    // Cached render pipeline ID.
    size_t pipeline;
    entt::entity entity;
    // Draw function ID.
    uint32_t draw_function;

    /// Range in the vertex buffer of this item
    std::optional<Range> batch_range;
};

class MainPass2dNode : public Node {
    entt::runtime_view view;

    static MainPass2dNode from_world(World& world) {
        //        return {world.query_filtered()};
        return {};
    }

    void update(World& world) {
        //        self.query.update_archetypes(world);
    }

    Result<int, NodeRunError> run(const RenderGraphContext& graph,
                                  RenderContext& render_context,
                                  const World& world) const override;
};

} // namespace Flint::Ecs

#endif // FLINT_RENDER_CORE_3D_H
