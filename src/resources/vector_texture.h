#ifndef FLINT_VECOTR_TEXTURE_H
#define FLINT_VECOTR_TEXTURE_H

#include "resource.h"

#include <memory>

class VectorTexture : public Resource {
public:
    VectorTexture() = default;

    /// Create vector texture from a svg file.
    explicit VectorTexture(const std::string &path);

    ~VectorTexture() override;

    /// Create an empty texture with specific size.
    static std::shared_ptr<VectorTexture> from_empty(uint32_t p_width, uint32_t p_height);

    //static std::shared_ptr<Texture> from_bytes(const std::string &filename);

    bool resource_ownership = true;

public:
    uint32_t width = 0;
    uint32_t height = 0;

private:
    void create_image_from_bytes(void *pixels, uint32_t tex_width, uint32_t tex_height);
};

#endif //FLINT_VECOTR_TEXTURE_H
