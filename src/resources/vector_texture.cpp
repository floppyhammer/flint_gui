#include <stdexcept>

#include "vector_texture.h"
#include "../common/logger.h"

#include <cassert>

namespace Flint {
    VectorTexture::VectorTexture() {
        type = TextureType::VECTOR;
    }

    VectorTexture::~VectorTexture() {
    }

    std::shared_ptr<VectorTexture> VectorTexture::from_empty(uint32_t p_width, uint32_t p_height) {
        assert(p_width != 0 && p_height != 0 && "Creating texture with zero size.");

        auto texture = std::make_shared<VectorTexture>();
        texture->width = p_width;
        texture->height = p_height;

        return texture;
    }

    VectorTexture::VectorTexture(const std::string &path) : Texture(path) {

    }

    void VectorTexture::add_to_canvas(const Vec2<float> &position,
                                      const std::shared_ptr<Pathfinder::Canvas> &canvas) {
        canvas->save_state();

        auto transform = Pathfinder::Transform2::from_translation({position.x, position.y});
        canvas->set_transform(transform);

        for (auto &s: svg_shapes) {
            canvas->set_line_width(s.stroke_width);
            canvas->set_stroke_paint(Pathfinder::Paint::from_color({s.stroke_color.r, s.stroke_color.g, s.stroke_color.b, s.stroke_color.a}));
            canvas->stroke_shape(s.shape);
        }

        canvas->restore_state();
    }

    void VectorTexture::add_svg_shape(SvgShape svg_shape) {
        svg_shapes.push_back(svg_shape);
    }
}
