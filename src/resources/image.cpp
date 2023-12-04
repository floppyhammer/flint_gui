#include "image.h"

namespace Flint {

Image::Image(const std::string &path) : Resource(path) {
}

Vec2I Image::get_size() {
    return size;
}

ImageType Image::get_type() {
    return type;
}

} // namespace Flint
