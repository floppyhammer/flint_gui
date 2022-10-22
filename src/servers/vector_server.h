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

    void set_render_target(const std::shared_ptr<ImageTexture> &dest_texture);

    void cleanup();

    void submit();

    /**
     * Enable content clip, portion of anything drawn afterward
     * outside the clip box will not show.
     */
    void set_content_clip_box(std::optional<RectF> clip_box);

    /**
     * Disable the previously set content clip box.
     */
    std::optional<RectF> get_content_clip_box();

    std::shared_ptr<ImageTexture> get_texture();

    void draw_line(Vec2F start, Vec2F end, float width, ColorU color);

    void draw_rectangle(const RectF &rect, float line_width, ColorU color);

    void draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color);

    void draw_path(VectorPath &vector_path, Transform2 transform);

    void draw_texture(VectorTexture &texture, Transform2 transform);

    void draw_style_box(const StyleBox &style_box, const Vec2F &position, const Vec2F &size);

    void draw_style_line(const StyleLine &style_line, const Vec2F &start, const Vec2F &end);

    void draw_glyphs(const std::vector<Glyph> &glyphs, FontStyle font_style, const Transform2 &global_transform);

    Vec2I default_canvas_size;

private:
    // Never expose this.
    std::shared_ptr<Pathfinder::Canvas> canvas;

    /// If set, paths drawn afterward will be clipped.
    /// The clip box is in the global coordinates.
    std::optional<RectF> content_clip_box;

private:
    /// Add clip path to canvas.
    void apply_content_clip_box();
};

} // namespace Flint

#endif // FLINT_VECTOR_SERVER_H
