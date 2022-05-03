#ifndef FLINT_TEXTURE_RECT_H
#define FLINT_TEXTURE_RECT_H

#include "control.h"
#include "../../../resources/texture.h"
#include "../../../resources/surface.h"

#include <memory>

namespace Flint {
    class TextureRect : public Control {
    public:
        TextureRect();

        void set_texture(std::shared_ptr<Texture> p_texture);
        [[nodiscard]] std::shared_ptr<Texture> get_texture() const;

    private:
        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        std::shared_ptr<Mesh2d> mesh;
    };
}

#endif //FLINT_TEXTURE_RECT_H
