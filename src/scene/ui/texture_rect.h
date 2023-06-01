#ifndef FLINT_TEXTURE_RECT_H
#define FLINT_TEXTURE_RECT_H

#include <memory>

#include "../../resources/surface.h"
#include "../../resources/texture.h"
#include "node_ui.h"

namespace Flint {

/**
 * Used to display an ImageTexture or a VectorTexture.
 */
class TextureRect : public NodeUi {
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
        KeepCovered, // Not available for VectorTexture.
        Tile,        // Do not work.
        Max,
    };

    void set_stretch_mode(StretchMode new_stretch_mode);

    void draw() override;

private:
    void update(double delta) override;

    StretchMode stretch_mode = StretchMode::Scale;

    std::shared_ptr<Texture> texture;
};

} // namespace Flint

#endif // FLINT_TEXTURE_RECT_H
