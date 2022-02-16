#ifndef FLINT_TEXTURE_RECT_H
#define FLINT_TEXTURE_RECT_H

#include "control.h"
#include "../../rendering/texture.h"

#include <memory>

namespace Flint {
    class TextureRect : public Control {
    public:
        TextureRect(float viewport_width, float viewport_height);

        ~TextureRect();

        void set_texture(std::shared_ptr<Texture> p_texture);

        std::shared_ptr<Texture> get_texture() const;

        void self_draw() override;

    private:
        std::shared_ptr<Texture> texture;

        unsigned int vao = 0;
        unsigned int vbo = 0;
    };
}

#endif //FLINT_TEXTURE_RECT_H
