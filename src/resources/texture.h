#ifndef FLINT_TEXTURE_H
#define FLINT_TEXTURE_H

#include "../common/geometry.h"
#include "resource.h"

namespace Flint {

enum class TextureType {
    Image,
    Vector,
    Cube,
    Max,
};

/// A texture's layout is always SHADER_READ_ONLY.
class Texture : public Resource {
public:
    Texture() = default;

    explicit Texture(const std::string &path);

    Vec2I get_size();

    TextureType get_type();

protected:
    TextureType type = TextureType::Max;

    Vec2I size;
};

} // namespace Flint

#endif // FLINT_TEXTURE_H
