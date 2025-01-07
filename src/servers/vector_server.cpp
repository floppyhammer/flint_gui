#include "vector_server.h"

namespace Flint {

constexpr float STROKE_WIDTH_FOR_PSEUDO_BOLD_TEXT = 1.0;

void VectorServer::init(Pathfinder::Vec2I size,
                        const std::shared_ptr<Pathfinder::Device> &device,
                        const std::shared_ptr<Pathfinder::Queue> &queue,
                        Pathfinder::RenderLevel level) {
    canvas = std::make_shared<Pathfinder::Canvas>(size, device, queue, level);

    reset_render_layers();
}

void VectorServer::cleanup() {
    canvas.reset();
}

void VectorServer::set_dst_texture(const std::shared_ptr<Pathfinder::Texture> &texture) {
    canvas->set_dst_texture(texture);
}

void VectorServer::submit_and_clear() {
    for (uint8_t i = 0; i < MAX_RENDER_LAYER; i++) {
        canvas->set_scene(render_layers[i]);
        canvas->draw(i == 0);
    }

    reset_render_layers();
}

std::shared_ptr<Pathfinder::Canvas> VectorServer::get_canvas() const {
    return canvas;
}

float VectorServer::get_global_scale() const {
    return global_scale_;
}

void VectorServer::set_global_scale(float new_scale) {
    global_scale_ = new_scale;
}

void VectorServer::set_render_layer(uint8_t layer_id) {
    assert(layer_id <= MAX_RENDER_LAYER);
    if (layer_id >= MAX_RENDER_LAYER) {
        return;
    }

    canvas->set_scene(render_layers[layer_id]);
}

void VectorServer::reset_render_layers() {
    for (uint8_t i = 0; i < MAX_RENDER_LAYER; i++) {
        render_layers[i] = std::make_shared<Pathfinder::Scene>(0, RectF({}, canvas->get_size().to_f32()));
    }
    canvas->set_scene(render_layers[0]);
}

void VectorServer::draw_line(Vec2F start, Vec2F end, float width, ColorU color) {
    canvas->save_state();

    Pathfinder::Path2d path;
    path.add_line({start.x, start.y}, {end.x, end.y});

    canvas->set_stroke_paint(Pathfinder::Paint::from_color(color));
    canvas->set_line_width(width);
    // canvas->set_line_cap(Pathfinder::LineCap::Round);
    canvas->stroke_path(path);

    canvas->restore_state();
}

void VectorServer::draw_rectangle(const RectF &rect, float line_width, ColorU color, bool fill) {
    canvas->save_state();

    Pathfinder::Path2d path;
    path.add_rect(rect);

    canvas->set_transform(Pathfinder::Transform2::from_scale(Vec2F(global_scale_, global_scale_)));

    if (fill) {
        canvas->set_fill_paint(Pathfinder::Paint::from_color(color));
        canvas->fill_path(path, Pathfinder::FillRule::Winding);
    } else {
        canvas->set_stroke_paint(Pathfinder::Paint::from_color(color));
        canvas->set_line_width(line_width);
        canvas->stroke_path(path);
    }

    canvas->restore_state();
}

void VectorServer::draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color) {
    canvas->save_state();

    Pathfinder::Path2d path;
    path.add_circle({center.x, center.y}, radius);

    if (fill) {
        canvas->set_fill_paint(Pathfinder::Paint::from_color(color));
        canvas->fill_path(path, Pathfinder::FillRule::Winding);
    } else if (line_width > 0) {
        canvas->set_stroke_paint(Pathfinder::Paint::from_color(color));
        canvas->set_line_width(line_width);
        canvas->stroke_path(path);
    }

    canvas->restore_state();
}

void VectorServer::draw_path(VectorPath &vector_path, Transform2 transform) {
    canvas->save_state();

    auto dpi_scaling_xform = Pathfinder::Transform2::from_scale(Vec2F(global_scale_, global_scale_));

    canvas->set_transform(dpi_scaling_xform * global_transform_offset * transform);

    if (vector_path.fill_color.is_opaque()) {
        canvas->set_fill_paint(Pathfinder::Paint::from_color(vector_path.fill_color));
        canvas->fill_path(vector_path.path2d, Pathfinder::FillRule::Winding);
    }

    if (vector_path.stroke_width > 0) {
        canvas->set_stroke_paint(Pathfinder::Paint::from_color(vector_path.stroke_color));
        canvas->set_line_width(vector_path.stroke_width);
        canvas->stroke_path(vector_path.path2d);
    }

    canvas->restore_state();
}

void VectorServer::draw_raster_image(RasterImage &image, Transform2 transform) {
    canvas->save_state();

    auto dpi_scaling_xform = Pathfinder::Transform2::from_scale(Vec2F(global_scale_, global_scale_));

    canvas->set_transform(dpi_scaling_xform * global_transform_offset * transform);

    auto image_data = image.image_data;

    canvas->draw_image(image_data, RectF({}, Vec2F() + image_data->size.to_f32()));

    canvas->restore_state();
}

void VectorServer::draw_vector_image(VectorImage &image, Transform2 transform) {
    auto dpi_scaling_xform = Pathfinder::Transform2::from_scale(Vec2F(global_scale_, global_scale_));

    for (auto &path : image.get_paths()) {
        draw_path(path, dpi_scaling_xform * transform);
    }

    if (image.get_svg_scene()) {
        canvas->get_scene()->append_scene(*image.get_svg_scene()->get_scene(),
                                          dpi_scaling_xform * global_transform_offset * transform);
    }
}

void VectorServer::draw_render_image(RenderImage &render_image, Transform2 transform) {
    canvas->save_state();

    auto dpi_scaling_xform = Pathfinder::Transform2::from_scale(Vec2F(global_scale_, global_scale_));

    canvas->set_transform(dpi_scaling_xform * global_transform_offset * transform);

    canvas->draw_raw_texture(render_image.get_texture(), RectF({}, render_image.get_size().to_f32()));

    canvas->restore_state();
}

void VectorServer::draw_style_box(const StyleBox &style_box, const Vec2F &position, const Vec2F &size) {
    auto path = Pathfinder::Path2d();
    if (style_box.corner_radii.has_value()) {
        path.add_rect_with_corners({{}, size}, style_box.corner_radii.value());
    } else {
        path.add_rect({{}, size}, style_box.corner_radius);
    }

    canvas->save_state();

    canvas->set_shadow_color(style_box.shadow_color);
    canvas->set_shadow_blur(style_box.shadow_size);

    auto dpi_scaling_xform = Pathfinder::Transform2::from_scale(Vec2F(global_scale_, global_scale_));

    auto transform = Pathfinder::Transform2::from_translation(position);
    canvas->set_transform(dpi_scaling_xform * global_transform_offset * transform);

    canvas->set_fill_paint(Pathfinder::Paint::from_color(style_box.bg_color));
    canvas->fill_path(path, Pathfinder::FillRule::Winding);

    if (style_box.border_width > 0) {
        canvas->set_stroke_paint(Pathfinder::Paint::from_color(style_box.border_color));
        canvas->set_line_width(style_box.border_width);
        canvas->stroke_path(path);
    }

    canvas->restore_state();
}

void VectorServer::draw_style_line(const StyleLine &style_line, const Vec2F &start, const Vec2F &end) {
    auto path = Pathfinder::Path2d();
    path.add_line(start, end);

    canvas->save_state();

    auto dpi_scaling_xform = Pathfinder::Transform2::from_scale(Vec2F(global_scale_, global_scale_));

    canvas->set_transform(dpi_scaling_xform * global_transform_offset);
    canvas->set_stroke_paint(Pathfinder::Paint::from_color(style_line.color));
    canvas->set_line_width(style_line.width);
    canvas->stroke_path(path);

    canvas->restore_state();
}

void VectorServer::draw_glyphs(std::vector<Glyph> &glyphs,
                               std::vector<Vec2F> &glyph_positions,
                               TextStyle text_style,
                               const Transform2 &transform,
                               const RectF &clip_box) {
    if (glyphs.size() != glyph_positions.size()) {
        Logger::error("Glyph count mismatches glyph position count!", "VectorServer");
        return;
    }

    canvas->save_state();

    auto dpi_scaling_xform = Pathfinder::Transform2::from_scale(Vec2F(global_scale_, global_scale_));

    // Text clip.
    if (clip_box.is_valid()) {
        auto clip_path = Pathfinder::Path2d();
        clip_path.add_rect(clip_box, 0);
        canvas->set_transform(dpi_scaling_xform * global_transform_offset * transform);
        canvas->clip_path(clip_path, Pathfinder::FillRule::Winding);
    }

    auto skew_xform = Transform2::from_scale({1, 1});
    if (text_style.italic) {
        skew_xform = Transform2({1, 0, std::tan(-15.f * 3.1415926f / 180.f), 1}, {});
    }

    // Draw glyph strokes. The strokes go below the fills.
    for (int i = 0; i < glyphs.size(); i++) {
        auto &g = glyphs[i];
        auto &p = glyph_positions[i];

        if (g.emoji || g.skip_drawing) {
            continue;
        }

        auto baseline_xform = Transform2::from_translation({0, g.ascent});

        auto glyph_global_transform =
            dpi_scaling_xform * global_transform_offset * Transform2::from_translation(p) * transform * baseline_xform;

        canvas->set_transform(glyph_global_transform * skew_xform);

        // Add stroke if needed.
        canvas->set_stroke_paint(Pathfinder::Paint::from_color(text_style.stroke_color));
        float stroke_width = text_style.stroke_width;
        if (text_style.bold) {
            stroke_width += STROKE_WIDTH_FOR_PSEUDO_BOLD_TEXT;
        }
        canvas->set_line_width(stroke_width);
        canvas->set_line_join(Pathfinder::LineJoin::Round);
        canvas->stroke_path(g.path);
    }

    // Draw glyph fills.
    for (int i = 0; i < glyphs.size(); i++) {
        auto &g = glyphs[i];
        auto &p = glyph_positions[i];

        if (g.skip_drawing) {
            continue;
        }

        auto baseline_xform = Transform2::from_translation({0, g.ascent});

        // No italic for emojis and debug boxes.
        auto glyph_global_transform =
            dpi_scaling_xform * global_transform_offset * Transform2::from_translation(p) * transform * baseline_xform;

        if (!g.emoji) {
            canvas->set_transform(glyph_global_transform * skew_xform);

            // Add fill.
            canvas->set_fill_paint(Pathfinder::Paint::from_color(text_style.color));
            canvas->fill_path(g.path, Pathfinder::FillRule::Winding);

            // Use stroke to make a pseudo bold effect.
            if (text_style.bold) {
                canvas->set_stroke_paint(Pathfinder::Paint::from_color(text_style.color));
                canvas->set_line_width(STROKE_WIDTH_FOR_PSEUDO_BOLD_TEXT);
                canvas->set_line_join(Pathfinder::LineJoin::Bevel);
                canvas->stroke_path(g.path);
            }
        } else {
            auto svg_scene = std::make_shared<Pathfinder::SvgScene>(g.svg, *canvas);

            // The emoji's svg size is always fixed for a specific font no matter what the font size you set.
            auto svg_size = svg_scene->get_size();
            auto glyph_size = g.box.size();

            auto emoji_scale = Transform2::from_scale(glyph_size / svg_size);

            canvas->get_scene()->append_scene(*(svg_scene->get_scene()), glyph_global_transform * emoji_scale);
        }

        if (text_style.debug) {
            canvas->set_transform(glyph_global_transform);
            canvas->set_line_width(1);

            // Add box.
            // --------------------------------
            Pathfinder::Path2d layout_path;
            layout_path.add_rect(g.box);

            canvas->set_stroke_paint(Pathfinder::Paint::from_color(ColorU::green()));
            canvas->stroke_path(layout_path);
            // --------------------------------

            // Add bbox.
            // --------------------------------
            Pathfinder::Path2d bbox_path;
            bbox_path.add_rect(g.bbox);

            canvas->set_stroke_paint(Pathfinder::Paint::from_color(ColorU::red()));
            canvas->stroke_path(bbox_path);
            // --------------------------------
        }
    }

    canvas->restore_state();
}

std::shared_ptr<Pathfinder::SvgScene> VectorServer::load_svg(const std::string &path) {
    auto bytes = Pathfinder::load_file_as_string(path);

    auto svg_scene = std::make_shared<Pathfinder::SvgScene>(bytes, *canvas);

    return svg_scene;
}

} // namespace Flint
