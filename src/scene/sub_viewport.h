#ifndef FLINT_SUB_VIEWPORT_H
#define FLINT_SUB_VIEWPORT_H

#include "../common/geometry.h"
#include "../render/render_server.h"
#include "../render/render_target.h"
#include "../resources/image_texture.h"
#include "node.h"

using Pathfinder::ColorF;
using Pathfinder::Vec2I;

namespace Flint {

class Camera3d;
class Camera2d;

/**
 * A thin node wrapper over the render target.
 */
class SubViewport : public Node {
public:
    SubViewport();

    std::shared_ptr<ImageTexture> get_texture() const;

    Vec2I get_extent() const;

    std::shared_ptr<RenderTarget> render_target;

    ColorF clear_color = ColorF(0.1, 0.2, 0.3, 1.0);

    float fov = 45.0;

    float z_near = 0.1;
    float z_far = 10.0;

    bool is_2d = true;

    void propagate_draw(VkCommandBuffer cmd_buffer) override;

    void set_camera2d(Camera2d* _camera2d);

    void set_camera3d(Camera3d* _camera3d);

    void get_view_uniform() const;

private:
    // Active camera in this viewport. Only one is valid.
    Camera3d* camera3d{};
    Camera2d* camera2d{};
};

} // namespace Flint

#endif // FLINT_SUB_VIEWPORT_H
