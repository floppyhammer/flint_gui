#ifndef FLINT_UI_LAYER_H
#define FLINT_UI_LAYER_H

#include <memory>
#include <vector>

#include "node.h"
#include "resources/mesh.h"

namespace Flint::Scene {

/// UiLayer will draw every child UI nodes directly to the screen.
class UiLayer : public Node {
public:
    UiLayer();

    void draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    void propagate_draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    void update_mvp();

    void when_window_size_changed(Vec2I new_size);

    std::shared_ptr<Mesh2d> mesh;

    MvpPushConstant push_constant;

    std::shared_ptr<Pathfinder::Scene> vector_scene;

    std::shared_ptr<ImageTexture> texture;
};

} // namespace Flint

#endif // FLINT_UI_LAYER_H
