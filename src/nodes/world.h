#ifndef FLINT_WORLD_H
#define FLINT_WORLD_H

#include "../common/geometry.h"
#include "../render/mvp.h"
#include "../render/subview.h"
#include "../resources/image_texture.h"
#include "../resources/surface.h"
#include "../servers/render_server.h"
#include "node.h"

using Pathfinder::ColorF;
using Pathfinder::Vec2I;

namespace Flint {

class Camera3d;
class Camera2d;

/// You can have multiple worlds which contain multiple cameras at the same time.
/// However, a Camera2d can only be effective in a 2d world. And the same applies to a Camera3d.
/// UI nodes have nothing to do with World.
class World : public Node {
    friend class SceneTree;

public:
    World(bool _is_2d);

    void propagate_draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    void add_camera2d(Camera2d* new_camera);

    void add_camera3d(Camera3d* new_camera);

    Camera3d* current_camera3d{};
    Camera2d* current_camera2d{};

    void draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    void update_mvp();

    std::shared_ptr<Surface2d> surface;

    Vec2I view_size;

    MvpPushConstant push_constant;

private:
    bool is_2d;

    // Active cameras in this world. Only one is valid.
    std::vector<Camera3d*> camera3ds{};
    std::vector<Camera2d*> camera2ds{};

private:
    void draw_subtree(Subview* subview, ColorF clear_color, ImageTexture* image_texture);
};

} // namespace Flint

#endif // FLINT_WORLD_H
