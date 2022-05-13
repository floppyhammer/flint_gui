#include <stdexcept>

#include "vector_texture.h"
#include "../common/logger.h"

#include <cassert>

VectorTexture::~VectorTexture() {
}

std::shared_ptr<VectorTexture> VectorTexture::from_empty(uint32_t p_width, uint32_t p_height) {
    assert(p_width != 0 && p_height != 0 && "Creating texture with zero size.");

    auto texture = std::make_shared<VectorTexture>();
    texture->width = p_width;
    texture->height = p_height;

    return texture;
}

VectorTexture::VectorTexture(const std::string &path) : Texture(path) {

}
