#include "vector_texture.h"

#include <nanosvg.h>

#include <cassert>
#include <stdexcept>
#include <utility>

#include "../common/io.h"
#include "../common/logger.h"

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
        Pathfinder::Outline outline;

        // Load the bounds from the SVG file, will be modified when pushing points anyway.
        outline.bounds = Pathfinder::Rect<float>(nsvg_shape->bounds);

        for (NSVGpath *nsvg_path = nsvg_shape->paths; nsvg_path != nullptr; nsvg_path = nsvg_path->next) {
            outline.move_to(nsvg_path->pts[0], nsvg_path->pts[1]);

            // -6 or -3, both will do, probably.
            for (int point_index = 0; point_index < nsvg_path->npts - 3; point_index += 3) {
                // * 2 because a point has x and y components.
                float *p = &nsvg_path->pts[point_index * 2];
                outline.cubic_to(p[2], p[3], p[4], p[5], p[6], p[7]);
            }

            if (nsvg_path->closed) outline.close();
        }

        VectorPath vp;
        vp.outline = outline;

        // Add fill.
        vp.fill_color = ColorU(nsvg_shape->fill.color);

        // Add stroke if needed.
        vp.stroke_color = ColorU(nsvg_shape->stroke.color);
        vp.stroke_width = nsvg_shape->strokeWidth;

        vector_paths.push_back(vp);
    }

    // Clean up NanoSVG.
    nsvgDelete(image);
}

void VectorTexture::add_to_canvas(const Vec2<float> &position, const std::shared_ptr<Pathfinder::Canvas> &canvas) {
    canvas->save_state();

    auto transform = Pathfinder::Transform2::from_translation({position.x, position.y});
    canvas->set_transform(transform);

    for (auto &vp : vector_paths) {
        canvas->set_fill_paint(Pathfinder::Paint::from_color({vp.fill_color}));
        canvas->fill_path(vp.outline, Pathfinder::FillRule::Winding);

        canvas->set_line_width(vp.stroke_width);
        canvas->set_stroke_paint(Pathfinder::Paint::from_color({vp.stroke_color}));
        canvas->stroke_path(vp.outline);
    }

    canvas->restore_state();
}

void VectorTexture::set_vector_paths(const std::vector<VectorPath> &p_vector_paths) {
    vector_paths = p_vector_paths;
}
} // namespace Flint
