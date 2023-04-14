#include "vector_texture.h"

#include <cassert>
#include <stdexcept>
#include <utility>

#include "../common/load_file.h"
#include "../common/utils.h"
#include "../servers/vector_server.h"

using std::vector;

namespace Flint {

VectorTexture::VectorTexture(Vec2I _size) {
    type = TextureType::Vector;

    size = _size;
}

std::shared_ptr<VectorTexture> VectorTexture::from_empty(Vec2I _size) {
    assert(_size.area() != 0 && "Creating texture with zero size!");

    auto texture = std::make_shared<VectorTexture>(_size);

    return texture;
}

VectorTexture::VectorTexture(const std::string &path) : Texture(path) {
    svg_scene = VectorServer::get_singleton()->load_svg(path);

    size = svg_scene->get_scene()->get_view_box().size().to_i32();
}

void VectorTexture::add_path(const VectorPath &new_path) {
    paths.push_back(new_path);
}

vector<VectorPath> &VectorTexture::get_paths() {
    return paths;
}

shared_ptr<Pathfinder::SvgScene> VectorTexture::get_svg_scene() {
    return svg_scene;
}

} // namespace Flint
