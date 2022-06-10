#ifndef FLINT_TEXTURE_RECT_H
#define FLINT_TEXTURE_RECT_H

#include "control.h"
#include "../../../resources/texture.h"
#include "../../../resources/surface.h"

#include <memory>

namespace Flint {
    /**
     * Used to display an ImageTexture or a VectorTexture.
     */
    class TextureRect : public Control {
    public:
        TextureRect();

        void set_texture(const std::shared_ptr<Texture> &p_texture);

        [[nodiscard]] std::shared_ptr<Texture> get_texture() const;

        virtual Vec2<float> calculate_minimum_size() const override;

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

        Surface2dPushConstant push_constant;

        std::shared_ptr<Texture> texture;
    };
}

#endif //FLINT_TEXTURE_RECT_H
