#ifndef FLINT_VECTOR_TEXTURE_H
#define FLINT_VECTOR_TEXTURE_H

#include "texture.h"

#include <memory>

class VectorTexture : public Texture {
public:
    VectorTexture() = default;

    /// Create vector texture from a svg file.
    explicit VectorTexture(const std::string &path);

    ~VectorTexture() override;

    /// Create an empty texture with specific size.
    static std::shared_ptr<VectorTexture> from_empty(uint32_t p_width, uint32_t p_height);
};

#endif //FLINT_VECTOR_TEXTURE_H
