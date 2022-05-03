#ifndef FLINT_SUB_VIEWPORT_H
#define FLINT_SUB_VIEWPORT_H

#include "node.h"
#include "../../common/vec2.h"
#include "../../render/render_server.h"
#include "../../resources/texture.h"
#include "../../render/viewport.h"
#include "scene_tree.h"
#include "../../common/color.h"

namespace Flint {
    /**
     * A thin wrapper over rendering Viewport.
     */
    class SubViewport : public Node {
    public:
        SubViewport();

        [[nodiscard]] std::shared_ptr<Texture> get_texture() const;

        [[nodiscard]] Vec2<uint32_t> get_extent() const;

        std::shared_ptr<Viewport> viewport;

        ColorF clear_color = ColorF(0.1, 0.2, 0.3, 1.0);

        float fov = 45.0;

        float z_near = 0.1;
        float z_far = 10.0;

        void propagate_draw(VkCommandBuffer p_command_buffer) override;

    protected:
    };
}

#endif //FLINT_SUB_VIEWPORT_H
