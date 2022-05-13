#include "texture.h"

namespace Flint {
    Texture::Texture(const std::string &path) : Resource(path) {

    }

    uint32_t Texture::get_width() {
        return width;
    }

    uint32_t Texture::get_height() {
        return height;
    }

    TextureType Texture::get_type() {
        return type;
    }
}
