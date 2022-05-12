#ifndef FLINT_TEXTURE_RECT_H
#define FLINT_TEXTURE_RECT_H

#include "control.h"
#include "../../../resources/image_texture.h"
#include "../../../resources/surface.h"

#include <memory>

namespace Flint {
    /**
     * TextureRect can be used to draw ImageTexture and VectorTexture.
     */
    class TextureRect : public Control {
    public:
        TextureRect();

        void set_texture(std::shared_ptr<ImageTexture> p_texture);
        [[nodiscard]] std::shared_ptr<ImageTexture> get_texture() const;

    private:
        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        void update_mvp();

        std::shared_ptr<Mesh2d> mesh;

        Surface2dPushConstant push_constant;

        std::shared_ptr<ImageTexture> texture;
    };
}

#endif //FLINT_TEXTURE_RECT_H
