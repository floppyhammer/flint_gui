#ifndef FLINT_VECTOR_SERVER_H
#define FLINT_VECTOR_SERVER_H

#include <pathfinder.h>

#include "../common/geometry.h"
#include "../resources/font.h"
#include "../resources/image_texture.h"
#include "../resources/vector_texture.h"
#include "resources/style_box.h"

using Pathfinder::ColorU;
using Pathfinder::RectF;
using Pathfinder::Transform2;
using Pathfinder::Vec2F;
using Pathfinder::Vec2I;

namespace Flint {

/**
 * All visible shapes will be collected by the vector server and drawn at once.
 */
class VectorServer {
public:
    static VectorServer *get_singleton() {
        static VectorServer singleton;
        return &singleton;
    }

    VectorServer() = default;

    void init(const std::shared_ptr<Pathfinder::Driver> &driver, int p_canvas_width, int p_canvas_height);

    void cleanup();

    void submit();

    std::shared_ptr<ImageTexture> get_texture();

    void draw_line(Vec2F start, Vec2F end, float width, ColorU color);

    void draw_rectangle(const RectF &rect, float line_width, ColorU color);

    void draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color);

    void draw_path(VectorPath &vector_path, Transform2 transform);

    void draw_image_texture(ImageTexture &texture, Transform2 transform);

    void draw_vector_texture(VectorTexture &texture, Transform2 transform);

    void draw_style_box(const StyleBox &style_box, const Vec2F &position, const Vec2F &size);

    void draw_style_line(const StyleLine &style_line, const Vec2F &start, const Vec2F &end);

    /**
     * @param transform
     * @param clip_box Enable content clip, portion of anything drawn afterward
     * outside the clip box will not show. The rect is in local coordinates and the transform will be applied to it.
     * We shouldn't use clip path to achieve general content clip (like scrolling)
     * since it's quite performance heavy and easily produces nested clipping.
     */
    void draw_glyphs(const std::vector<Glyph> &glyphs,
                     FontStyle font_style,
                     const Transform2 &transform,
                     const RectF &clip_box);

    shared_ptr<Pathfinder::SvgScene> load_svg(const std::string &path);

    std::shared_ptr<Pathfinder::Canvas> get_canvas() const;

    Transform2 global_transform_offset;

private:
    // Never expose this.
    std::shared_ptr<Pathfinder::Canvas> canvas;
};

} // namespace Flint

#endif // FLINT_VECTOR_SERVER_H
