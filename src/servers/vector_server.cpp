#include "vector_server.h"

namespace Flint {
    void VectorServer::init(const std::shared_ptr<Pathfinder::Driver> &driver,
                            float p_canvas_width,
                            float p_canvas_height,
                            const std::vector<char> &area_lut_input) {
        canvas = std::make_shared<Pathfinder::Canvas>(driver,
                                                      p_canvas_width,
                                                      p_canvas_height,
                                                      area_lut_input);
    }

    void VectorServer::cleanup() {
        canvas.reset();
    }

    void VectorServer::clear() {
        canvas->clear();
    }

    void VectorServer::submit() {
        canvas->build_and_render();
    }

    void VectorServer::draw_line(Vec2F start, Vec2F end, float width, ColorU color) {
        Pathfinder::Outline outline;
        outline.add_line({start.x, start.y}, {end.x, end.y});

        canvas->save_state();
        canvas->set_stroke_paint(Pathfinder::Paint::from_color({color.r, color.g, color.b, color.a}));
        canvas->set_line_width(width);
        //canvas->set_line_cap(Pathfinder::LineCap::Round);
        canvas->stroke_path(outline);
        canvas->restore_state();
    }

    void VectorServer::draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color) {
        Pathfinder::Outline outline;
        outline.add_circle({center.x, center.y}, radius);

        canvas->save_state();

        if (fill) {
            canvas->set_fill_paint(Pathfinder::Paint::from_color({color.r, color.g, color.b, color.a}));
            canvas->fill_path(outline, Pathfinder::FillRule::Winding);
        } else if (line_width > 0) {
            canvas->set_stroke_paint(Pathfinder::Paint::from_color({color.r, color.g, color.b, color.a}));
            canvas->set_line_width(line_width);
            canvas->stroke_path(outline);
        }

        canvas->restore_state();
    }

    void VectorServer::draw_path(const VectorPath &vector_path, Transform2 transform) {
        canvas->save_state();

        canvas->set_transform(transform);

        if (vector_path.fill_color.is_opaque()) {
            canvas->set_fill_paint(Pathfinder::Paint::from_color(
                    {vector_path.fill_color.r,
                     vector_path.fill_color.g,
                     vector_path.fill_color.b,
                     vector_path.fill_color.a}));
            canvas->fill_path(vector_path.outline, Pathfinder::FillRule::Winding);
        }

        if (vector_path.stroke_width > 0) {
            canvas->set_stroke_paint(Pathfinder::Paint::from_color(
                    {vector_path.stroke_color.r,
                     vector_path.stroke_color.g,
                     vector_path.stroke_color.b,
                     vector_path.stroke_color.a}));
            canvas->set_line_width(vector_path.stroke_width);
            canvas->stroke_path(vector_path.outline);
        }

        canvas->restore_state();
    }

    std::shared_ptr<ImageTexture> VectorServer::get_texture() {
        auto texture_vk = static_cast<Pathfinder::TextureVk *>(canvas->get_dest_texture().get());
        return ImageTexture::from_wrapper(texture_vk->get_image_view(),
                                          texture_vk->get_sampler(),
                                          texture_vk->get_width(),
                                          texture_vk->get_height());
    }
}
