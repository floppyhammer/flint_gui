#include "texture.h"

namespace Flint {

Texture::Texture(const std::string &path) : Resource(path) {
}

Vec2I Texture::get_size() {
    return size;
}

TextureType Texture::get_type() {
    return type;
}

} // namespace Flint
