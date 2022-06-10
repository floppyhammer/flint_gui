#ifndef FLINT_TEXTURE_H
#define FLINT_TEXTURE_H

#include "resource.h"
#include "../common/geometry.h"

using Pathfinder::Vec2;

namespace Flint {
    enum class TextureType {
        IMAGE,
        VECTOR,
        MAX,
    };

    class Texture : public Resource {
    public:
        Texture() = default;

        explicit Texture(const std::string &path);

        uint32_t get_width();

        uint32_t get_height();

        Vec2 <uint32_t> get_size();

        TextureType get_type();

    protected:
        TextureType type = TextureType::MAX;

        uint32_t width = 0;
        uint32_t height = 0;
    };
}

#endif //FLINT_TEXTURE_H
