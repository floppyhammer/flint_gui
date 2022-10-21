#ifndef FLINT_TEXTURE_RECT_H
#define FLINT_TEXTURE_RECT_H

#include <memory>

#include "../../../resources/surface.h"
#include "../../../resources/texture.h"
#include "control.h"

namespace Flint {
/**
 * Used to display an ImageTexture or a VectorTexture.
 */
class TextureRect : public Control {
public:
    TextureRect();

    void set_texture(const std::shared_ptr<Texture> &p_texture);

    [[nodiscard]] std::shared_ptr<Texture> get_texture() const;

    virtual Vec2F calculate_minimum_size() const override;

    enum class StretchMode {
        SCALE,
        TILE,
        KEEP,
        KEEP_CENTER,
        KEEP_ASPECT,
    } stretch_mode = StretchMode::KEEP_CENTER;

private:
    void update(double delta) override;

    void draw(VkCommandBuffer p_command_buffer) override;

    void update_mvp();

    std::shared_ptr<Mesh2d> mesh;

    MvpPushConstant push_constant;

    std::shared_ptr<Texture> texture;
};
} // namespace Flint

#endif // FLINT_TEXTURE_RECT_H
