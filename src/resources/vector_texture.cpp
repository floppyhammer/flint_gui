#include <stdexcept>

#include "vector_texture.h"
#include "../common/logger.h"
#include "../common/io.h"

#include <nanosvg.h>

#include <cassert>
#include <utility>

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
        auto svg_string = load_file_as_string(path.c_str());

        // Load the SVG image via NanoSVG.
        NSVGimage *image;

        // Make a copy as nsvgParse() will empty the string.
        auto input_copy = svg_string;
        char *string_c = const_cast<char *>(input_copy.c_str());
        image = nsvgParse(string_c, "px", 96);

        // Check if image loading is successful.
        if (image == nullptr) {
            throw std::runtime_error(std::string("NanoSVG loading image failed."));
        }

        char buffer[100];
        snprintf(buffer, sizeof(buffer), "Loaded SVG image with size: (%.1f, %.1f)", image->width, image->height);
        Logger::verbose(std::string(buffer), "VectorTexture");

        width = image->width;
        height = image->height;

        // Extract shapes, paths and points from the SVG image.
        for (NSVGshape *nsvg_shape = image->shapes; nsvg_shape != nullptr; nsvg_shape = nsvg_shape->next) {
            Pathfinder::Shape shape;

            // Load the bounds from the SVG file, will be modified when pushing points anyway.
            shape.bounds = Pathfinder::Rect<float>(nsvg_shape->bounds[0],
                                       nsvg_shape->bounds[1],
                                       nsvg_shape->bounds[2],
                                       nsvg_shape->bounds[3]);

            for (NSVGpath *nsvg_path = nsvg_shape->paths; nsvg_path != nullptr; nsvg_path = nsvg_path->next) {
                shape.move_to(nsvg_path->pts[0], nsvg_path->pts[1]);

                // -6 or -3, both will do, probably.
                for (int point_index = 0; point_index < nsvg_path->npts - 3; point_index += 3) {
                    // * 2 because a point has x and y components.
                    float *p = &nsvg_path->pts[point_index * 2];
                    shape.cubic_to(p[2], p[3], p[4], p[5], p[6], p[7]);
                }

                if (nsvg_path->closed)
                    shape.close();
            }

            SvgShape svg_shape;
            svg_shape.shape = shape;

            // Add fill.
            svg_shape.fill_color = ColorU(nsvg_shape->fill.color);

            // Add stroke if needed.
            svg_shape.stroke_color = ColorU(nsvg_shape->stroke.color);
            svg_shape.stroke_width = nsvg_shape->strokeWidth;

            svg_shapes.push_back(svg_shape);
        }

        // Clean up NanoSVG.
        nsvgDelete(image);
    }

    void VectorTexture::add_to_canvas(const Vec2<float> &position,
                                      const std::shared_ptr<Pathfinder::Canvas> &canvas) {
        canvas->save_state();

        auto transform = Pathfinder::Transform2::from_translation({position.x, position.y});
        canvas->set_transform(transform);

        for (auto &s: svg_shapes) {
            canvas->set_fill_paint(Pathfinder::Paint::from_color({s.fill_color.r, s.fill_color.g, s.fill_color.b, s.fill_color.a}));
            canvas->fill_shape(s.shape, Pathfinder::FillRule::Winding);

            canvas->set_line_width(s.stroke_width);
            canvas->set_stroke_paint(Pathfinder::Paint::from_color({s.stroke_color.r, s.stroke_color.g, s.stroke_color.b, s.stroke_color.a}));
            canvas->stroke_shape(s.shape);
        }

        canvas->restore_state();
    }

    void VectorTexture::set_svg_shapes(std::vector<SvgShape> p_svg_shapes) {
        svg_shapes = std::move(p_svg_shapes);
    }
}
