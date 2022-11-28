#include "vector_texture.h"

#include <cassert>
#include <stdexcept>
#include <utility>

#include "../common/load_file.h"
#include "../common/logger.h"
#include "../servers/vector_server.h"

using std::vector;

namespace Flint {

VectorTexture::VectorTexture(uint32_t _width, uint32_t _height) {
    type = TextureType::VECTOR;

    width = _width;
    height = _height;
}

std::shared_ptr<VectorTexture> VectorTexture::from_empty(uint32_t _width, uint32_t _height) {
    assert(_width != 0 && _height != 0 && "Creating texture with zero size!");

    auto texture = std::make_shared<VectorTexture>(_width, _height);

    return texture;
}

VectorTexture::VectorTexture(const std::string &path) : Texture(path) {
    svg_scene = VectorServer::get_singleton()->load_svg(path);

    auto view_box_size = svg_scene->get_scene()->get_view_box().size();
    width = view_box_size.x;
    height = view_box_size.y;
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
