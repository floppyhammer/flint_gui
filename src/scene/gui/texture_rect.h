#ifndef FLINT_TEXTURE_RECT_H
#define FLINT_TEXTURE_RECT_H

#include <memory>

#include "control.h"
#include "resources/surface.h"
#include "resources/texture.h"

namespace Flint {
/**
 * Used to display an ImageTexture or a VectorTexture.
 */
class TextureRect : public Control {
public:
    TextureRect();

    void set_texture(const std::shared_ptr<Texture> &p_texture);

    [[nodiscard]] std::shared_ptr<Texture> get_texture() const;

    virtual Vec2F calc_minimum_size() const override;

    enum class StretchMode {
        Scale,        // Scale it to the TextureRect size.
        Keep,         // Do nothing.
        KeepCentered, // Center it but do not scale it.
        KeepAspect,
        KeepAspectCentered,
        KeepCovered,
        Tile, // Do not work.
        Max,
    };

    void set_stretch_mode(StretchMode new_stretch_mode);

private:
    void update(double delta) override;

    void draw(VkCommandBuffer p_command_buffer) override;

    void update_mvp();

    StretchMode stretch_mode = StretchMode::Scale;

    std::shared_ptr<Mesh2d> mesh;

    MvpPushConstant push_constant;

    std::shared_ptr<Texture> texture;
};
} // namespace Flint

#endif // FLINT_TEXTURE_RECT_H
