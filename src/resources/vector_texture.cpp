#include "vector_texture.h"

#include <cassert>
#include <stdexcept>
#include <utility>

#include "../common/io.h"
#include "../common/logger.h"

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
    auto svg_data = load_file_as_bytes(path.c_str());

//    auto canvas = VectorServer::get_singleton()->canvas;
//
//    svg_scene = std::make_shared<Pathfinder::SvgScene>();
//    svg_scene->load_file(svg_data, *canvas);


    //    auto canvas = VectorServer::get_singleton()->canvas;
    //
    //    auto old_scene = canvas->replace_scene(svg_scene->get_scene());
    //
    //    canvas->save_state();
    //
    //    auto transform = Pathfinder::Transform2::from_translation({position.x, position.y});
    //    canvas->set_transform(transform);
    //
    //    for (auto &vp : vector_paths) {
    //        canvas->set_fill_paint(Pathfinder::Paint::from_color({vp.fill_color}));
    //        canvas->fill_path(vp.path2d, Pathfinder::FillRule::Winding);
    //
    //        canvas->set_line_width(vp.stroke_width);
    //        canvas->set_stroke_paint(Pathfinder::Paint::from_color({vp.stroke_color}));
    //        canvas->stroke_path(vp.path2d);
    //    }
    //
    //    canvas->restore_state();
    //
    //    canvas->set_scene(old_scene);
}

void VectorTexture::add_path(const VectorPath &new_path) {
    paths.push_back(new_path);
}

std::vector<VectorPath> &VectorTexture::get_paths() {
    return paths;
}

} // namespace Flint
