#include "vector_image.h"

#include <cassert>
#include <stdexcept>
#include <utility>

#include "../common/load_file.h"
#include "../common/utils.h"
#include "../servers/vector_server.h"

namespace Flint {

VectorImage::VectorImage(Vec2I size_) : Image(size_) {
    type = ImageType::Vector;;
}

std::shared_ptr<VectorImage> VectorImage::from_empty(Vec2I _size) {
    assert(_size.area() != 0 && "Creating texture with zero size!");

    auto texture = std::make_shared<VectorImage>(_size);

    return texture;
}

VectorImage::VectorImage(const std::string &path) : Image(path) {
    svg_scene = VectorServer::get_singleton()->load_svg(path);

    size = svg_scene->get_size().to_i32();
}

void VectorImage::add_path(const VectorPath &new_path) {
    paths.push_back(new_path);
}

std::vector<VectorPath> &VectorImage::get_paths() {
    return paths;
}

std::shared_ptr<Pathfinder::SvgScene> VectorImage::get_svg_scene() {
    return svg_scene;
}

} // namespace Flint
