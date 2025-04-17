#pragma once

#include "../common/geometry.h"
#include "resource.h"

namespace revector {

enum class ImageType {
    Raster,
    Vector,
    Render,
    Max,
};

class Image : public Resource {
public:
    Image(Vec2I size_) {
        size = size_;
    }

    explicit Image(const std::string &path);

    Vec2I get_size();

    ImageType get_type();

protected:
    ImageType type = ImageType::Max;

    Vec2I size;
};

} // namespace revector
