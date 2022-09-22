#include "vector_server.h"

namespace Flint {
void VectorServer::init(const std::shared_ptr<Pathfinder::Driver> &driver,
                        float p_canvas_width,
                        float p_canvas_height) {
    canvas = std::make_shared<Pathfinder::Canvas>(driver);

    // TODO
    canvas->set_empty_dest_texture(p_canvas_width, p_canvas_height);

    default_canvas_size = {p_canvas_width, p_canvas_height};

    push_scene({0, 0, p_canvas_width, p_canvas_height});
}

void VectorServer::cleanup() {
    canvas.reset();
}

// TODO: It might not be a good idea to use view box for clipping.
// Viewport may be a better choice?
void VectorServer::push_scene(const Rect<float> &view_box) {
    canvas->set_empty_scene(view_box);
    scene_stack.push_back(canvas->get_scene_builder());
}

void VectorServer::pop_scene() {
    if (scene_stack.empty()) {
        return;
    }

    scene_stack.erase(scene_stack.end() - 1);

    if (scene_stack.empty()) {
        canvas->set_scene_builder(nullptr);
    } else {
        canvas->set_scene_builder(scene_stack.back());
    }
}

void VectorServer::clear_scene() {
    scene_stack.clear();
    canvas->clear();
    push_scene({0, 0, default_canvas_size.x, default_canvas_size.y});
}

void VectorServer::submit() {
    canvas->build_and_render();
}

void VectorServer::draw_line(Vec2F start, Vec2F end, float width, ColorU color) {
    if (scene_stack.empty()) {
        return;
    }

    canvas->set_scene_builder(scene_stack.back());

    Pathfinder::Outline outline;
    outline.add_line({start.x, start.y}, {end.x, end.y});

    canvas->save_state();
    canvas->set_stroke_paint(Pathfinder::Paint::from_color({color.r, color.g, color.b, color.a}));
    canvas->set_line_width(width);
    // canvas->set_line_cap(Pathfinder::LineCap::Round);
    canvas->stroke_path(outline);
    canvas->restore_state();
}

void VectorServer::draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color) {
    if (scene_stack.empty()) {
        return;
    }

    canvas->set_scene_builder(scene_stack.back());

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
    if (scene_stack.empty()) {
        return;
    }

    canvas->set_scene_builder(scene_stack.back());

    canvas->save_state();

    canvas->set_transform(transform);

    if (vector_path.fill_color.is_opaque()) {
        canvas->set_fill_paint(Pathfinder::Paint::from_color(
            {vector_path.fill_color.r, vector_path.fill_color.g, vector_path.fill_color.b, vector_path.fill_color.a}));
        canvas->fill_path(vector_path.outline, Pathfinder::FillRule::Winding);
    }

    if (vector_path.stroke_width > 0) {
        canvas->set_stroke_paint(Pathfinder::Paint::from_color({vector_path.stroke_color.r,
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
    return ImageTexture::from_wrapper(
        texture_vk->get_image_view(), texture_vk->get_sampler(), texture_vk->get_width(), texture_vk->get_height());
}

void VectorServer::set_render_target(std::shared_ptr<ImageTexture> dest_texture) {
}
} // namespace Flint
