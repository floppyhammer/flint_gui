#include "texture_rect.h"

namespace Flint {
    TextureRect::TextureRect(float viewport_width, float viewport_height) {
        rect_size.x = viewport_width;
        rect_size.y = viewport_height;

        // Set VAO&VBO.
        // ---------------------------
        // Set up vertex data (and buffer(s)) and configure vertex attributes.
        float vertices[] = {
                // Positions, colors, UVs.
                -1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
                1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
                1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0,

                -1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
                1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0,
                -1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0
        };
    }

    TextureRect::~TextureRect() {
    }

    void TextureRect::set_texture(std::shared_ptr<Texture> p_texture) {
        texture = std::move(p_texture);
    }

    std::shared_ptr<Texture> TextureRect::get_texture() const {
        return texture;
    }

    void TextureRect::self_draw() {

    }
}
