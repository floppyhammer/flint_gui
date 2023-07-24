#ifndef FLINT_SPRITE2D_H
#define FLINT_SPRITE2D_H

#include "../../render/mvp.h"
#include "../../resources/surface.h"
#include "node2d.h"

namespace Flint {

class Sprite2d : public Node2d {
public:
    Sprite2d();

    void set_texture(std::shared_ptr<ImageTexture> _texture);

    [[nodiscard]] std::shared_ptr<ImageTexture> get_texture() const;

    void set_surface(const std::shared_ptr<Surface2d> &_surface);

    void set_material(const std::shared_ptr<Material2d> &_material);

private:
    void update(double delta) override;

    void draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    void update_mvp();

    MvpPushConstant push_constant;

    std::shared_ptr<ImageTexture> texture;
};

} // namespace Flint

#endif // FLINT_SPRITE2D_H
