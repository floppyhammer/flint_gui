#include "vector_server.h"

namespace Flint {

void VectorServer::init(const std::shared_ptr<Pathfinder::Driver> &driver, int canvas_width, int canvas_height) {
    canvas = std::make_shared<Pathfinder::Canvas>(driver);
    canvas->set_new_dst_texture({canvas_width, canvas_height});
}

void VectorServer::cleanup() {
    canvas.reset();
}

void VectorServer::submit() {
    canvas->draw();
    canvas->clear();

    // Get the dst texture.
    auto texture_vk = static_cast<Pathfinder::TextureVk *>(canvas->get_dst_texture().get());

    // Transition the dst texture to ShaderReadOnly layout, so we can use it as a sampler.
    // Its layout may be Undefined or ColorAttachment.
    RenderServer::getSingleton()->transitionImageLayout(texture_vk->get_image(),
                                                        Pathfinder::to_vk_texture_format(texture_vk->get_format()),
                                                        Pathfinder::to_vk_layout(texture_vk->get_layout()),
                                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                        1,
                                                        1);
    texture_vk->set_layout(Pathfinder::TextureLayout::ShaderReadOnly);
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

void VectorServer::draw_rectangle(const RectF &rect, float line_width, ColorU color) {
    canvas->save_state();

    Pathfinder::Path2d path;
    path.add_rect(rect);

    canvas->set_stroke_paint(Pathfinder::Paint::from_color(color));
    canvas->set_line_width(line_width);
    canvas->stroke_path(path);

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

    canvas->set_transform(global_transform_offset * transform);

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

void VectorServer::draw_image_texture(ImageTexture &texture, Transform2 transform) {
    Pathfinder::Vec2F pos = {100, 200};
    //    canvas->draw_image(image, Pathfinder::RectF(pos, pos + image.size.to_f32()));
}

void VectorServer::draw_vector_texture(VectorTexture &texture, Transform2 transform) {
    for (auto &path : texture.get_paths()) {
        draw_path(path, transform);
    }

    if (texture.get_svg_scene()) {
        canvas->get_scene()->append_scene(*texture.get_svg_scene()->get_scene(), global_transform_offset * transform);
    }
}

void VectorServer::draw_style_box(const StyleBox &style_box, const Vec2F &position, const Vec2F &size) {
    auto path = Pathfinder::Path2d();
    path.add_rect({{}, size}, style_box.corner_radius);

    canvas->save_state();

    canvas->set_shadow_color(style_box.shadow_color);
    canvas->set_shadow_blur(style_box.shadow_size);

    auto transform = Pathfinder::Transform2::from_translation(position);
    canvas->set_transform(global_transform_offset * transform);

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

    canvas->set_transform(global_transform_offset);
    canvas->set_stroke_paint(Pathfinder::Paint::from_color(style_line.color));
    canvas->set_line_width(style_line.width);
    canvas->stroke_path(path);

    canvas->restore_state();
}

void VectorServer::draw_glyphs(const std::vector<Glyph> &glyphs,
                               FontStyle font_style,
                               const Transform2 &transform,
                               const RectF &clip_box) {
    canvas->save_state();

    // Text clip.
    if (clip_box.is_valid()) {
        auto clip_path = Pathfinder::Path2d();
        clip_path.add_rect(clip_box, 0);
        canvas->set_transform(global_transform_offset * transform);
        canvas->clip_path(clip_path, Pathfinder::FillRule::Winding);
    }

    // Draw glyphs.
    for (Glyph g : glyphs) {
        canvas->set_transform(global_transform_offset * Pathfinder::Transform2::from_translation(g.position) *
                              transform);

        // Add fill.
        canvas->set_fill_paint(Pathfinder::Paint::from_color(font_style.color));
        canvas->fill_path(g.path, Pathfinder::FillRule::Winding);

        // Add stroke if needed.
        canvas->set_stroke_paint(Pathfinder::Paint::from_color(font_style.stroke_color));
        canvas->set_line_width(font_style.stroke_width);
        canvas->stroke_path(g.path);

        if (font_style.debug) {
            canvas->set_line_width(1);

            // Add layout box.
            // --------------------------------
            Pathfinder::Path2d layout_path;
            layout_path.add_rect(g.layout_box);

            canvas->set_stroke_paint(Pathfinder::Paint::from_color(Pathfinder::ColorU::green()));
            canvas->stroke_path(layout_path);
            // --------------------------------

            // Add bbox.
            // --------------------------------
            Pathfinder::Path2d bbox_path;
            bbox_path.add_rect(g.bbox);

            canvas->set_stroke_paint(Pathfinder::Paint::from_color(Pathfinder::ColorU::red()));
            canvas->stroke_path(bbox_path);
            // --------------------------------
        }
    }

    canvas->restore_state();
}

shared_ptr<Pathfinder::SvgScene> VectorServer::load_svg(const std::string &path) {
    auto bytes = Pathfinder::load_file_as_bytes(path);

    auto svg_scene = std::make_shared<Pathfinder::SvgScene>();
    svg_scene->load_from_memory(bytes, *canvas);

    return svg_scene;
}

std::shared_ptr<ImageTexture> VectorServer::get_texture() {
    auto texture_vk = static_cast<Pathfinder::TextureVk *>(canvas->get_dst_texture().get());
    return ImageTexture::from_wrapper(texture_vk->get_image_view(), texture_vk->get_sampler(), texture_vk->get_size());
}

std::shared_ptr<Pathfinder::Canvas> VectorServer::get_canvas() const {
    return canvas;
}

} // namespace Flint
