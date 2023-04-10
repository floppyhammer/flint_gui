#ifndef FLINT_SPRITE2D_H
#define FLINT_SPRITE2D_H

#include "../../render/mvp.h"
#include "../../resources/mesh.h"
#include "node2d.h"

namespace Flint::Scene {

class Sprite2d : public Node2d {
public:
    Sprite2d();

    void set_texture(std::shared_ptr<ImageTexture> p_texture);

    [[nodiscard]] std::shared_ptr<ImageTexture> get_texture() const;

    void set_mesh(const std::shared_ptr<Mesh2d> &p_mesh);

    void set_material(const std::shared_ptr<Material2d> &p_material);

private:
    void update(double delta) override;

    void draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    void update_mvp();

    MvpPushConstant push_constant;

    std::shared_ptr<ImageTexture> texture;
};

} // namespace Flint

#endif // FLINT_SPRITE2D_H
