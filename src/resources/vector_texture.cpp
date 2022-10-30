#include "vector_texture.h"

#include <cassert>
#include <stdexcept>
#include <utility>

#include "../common/io.h"
#include "../common/logger.h"
#include "../servers/vector_server.h"

using std::vector;

namespace Flint {

VectorTexture::VectorTexture(uint32_t p_width, uint32_t p_height) {
    type = TextureType::VECTOR;

    width = p_width;
    height = p_height;
}

std::shared_ptr<VectorTexture> VectorTexture::from_empty(uint32_t p_width, uint32_t p_height) {
    assert(p_width != 0 && p_height != 0 && "Creating texture with zero size.");

    auto texture = std::make_shared<VectorTexture>(p_width, p_height);

    return texture;
}

VectorTexture::VectorTexture(const std::string &path) : Texture(path) {
    svg_scene = VectorServer::get_singleton()->load_svg(path);
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
