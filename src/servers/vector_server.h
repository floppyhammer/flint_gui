#pragma once

#include <pathfinder/prelude.h>

#include "../common/geometry.h"
#include "../resources/font.h"
#include "../resources/raster_image.h"
#include "../resources/render_image.h"
#include "../resources/style_box.h"
#include "../resources/vector_image.h"

namespace revector {

constexpr int MAX_RENDER_LAYER = 8;

/**
 * All visible shapes will be collected by the vector server and drawn at once.
 */
class VectorServer final {
public:
    static VectorServer *get_singleton() {
        static VectorServer singleton;
        return &singleton;
    }

    VectorServer() = default;

    void init(Pathfinder::Vec2I size,
              const std::shared_ptr<Pathfinder::Device> &device,
              const std::shared_ptr<Pathfinder::Queue> &queue,
              Pathfinder::RenderLevel level);

    void set_dst_texture(const std::shared_ptr<Pathfinder::Texture> &texture);

    void cleanup();

    void set_canvas_size(Vec2I new_size);

    void submit_and_clear();

    void draw_line(Vec2F start, Vec2F end, float width, ColorU color);

    void draw_rectangle(const RectF &rect, float line_width, ColorU color, bool fill);

    void draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color);

    void draw_path(VectorPath &vector_path, Transform2 transform);

    void draw_raster_image(RasterImage &image, Transform2 transform);

    void draw_vector_image(VectorImage &image, Transform2 transform);

    void draw_render_image(RenderImage &image, Transform2 transform);

    std::shared_ptr<Pathfinder::Texture> get_texture_by_render_target_id(Pathfinder::RenderTargetId id);

    void draw_style_box(const StyleBox &style_box, const Vec2F &position, const Vec2F &size, float alpha = 1.0f);

    void draw_style_line(const StyleLine &style_line, const Vec2F &start, const Vec2F &end);

    /**
     * @param transform
     * @param clip_box Enable content clip, portion of anything drawn afterward
     * outside the clip box will not show. The rect is in local coordinates and the transform will be applied to it.
     * We shouldn't use clip path to achieve general content clip (like scrolling)
     * since it's quite performance heavy and easily produces nested clipping.
     */
    void draw_glyphs(std::vector<Glyph> &glyphs,
                     std::vector<Vec2F> &glyph_positions,
                     TextStyle text_style,
                     const Transform2 &transform,
                     const RectF &clip_box,
                     float alpha = 1.0f);

    std::shared_ptr<Pathfinder::SvgScene> load_svg(const std::string &path);

    std::shared_ptr<Pathfinder::Canvas> get_canvas() const;

    float get_global_scale() const;

    void set_global_scale(float new_scale);

    void set_render_layer(uint8_t layer_id);

    // Only used with ScrollContainer.
    Transform2 global_transform_offset;

private:
    void reset_render_layers();

    // Never expose this.
    std::shared_ptr<Pathfinder::Canvas> canvas;

    std::array<std::shared_ptr<Pathfinder::Scene>, MAX_RENDER_LAYER> render_layers;

    float global_scale_ = 1.0f;
};

} // namespace revector
