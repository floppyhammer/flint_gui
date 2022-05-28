#ifndef FLINT_SUB_VIEWPORT_H
#define FLINT_SUB_VIEWPORT_H

#include "node.h"
#include "../../common/geometry.h"
#include "../../render/render_server.h"
#include "../../resources/image_texture.h"
#include "../../render/render_target.h"

using Pathfinder::ColorF;

namespace Flint {
    /**
     * A thin wrapper node of the render target.
     */
    class SubViewport : public Node {
    public:
        SubViewport();

        [[nodiscard]] std::shared_ptr<ImageTexture> get_texture() const;

        [[nodiscard]] Vec2<uint32_t> get_extent() const;

        std::shared_ptr<RenderTarget> render_target;

        ColorF clear_color = ColorF(0.1, 0.2, 0.3, 1.0);

        float fov = 45.0;

        float z_near = 0.1;
        float z_far = 10.0;

        void propagate_draw(VkCommandBuffer p_command_buffer) override;
    };
}

#endif //FLINT_SUB_VIEWPORT_H
