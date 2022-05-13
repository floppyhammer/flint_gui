#ifndef FLINT_TEXTURE_H
#define FLINT_TEXTURE_H

#include "resource.h"

class Texture : public Resource {
public:
    Texture() = default;

    explicit Texture(const std::string &path);

    uint32_t get_width();

    uint32_t get_height();

protected:
    uint32_t width = 0;
    uint32_t height = 0;
};

#endif //FLINT_TEXTURE_H
