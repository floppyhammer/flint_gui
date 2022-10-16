#include "vector_server.h"

#include "resources/vector_texture.h"

namespace Flint {

void VectorServer::init(const std::shared_ptr<Pathfinder::Driver> &driver, int p_canvas_width, int p_canvas_height) {
    canvas = std::make_shared<Pathfinder::Canvas>(driver);
    canvas->set_size({p_canvas_width, p_canvas_height});

    // TODO
    canvas->set_empty_dest_texture(p_canvas_width, p_canvas_height);

    default_canvas_size = {p_canvas_width, p_canvas_height};

    //    push_scene({0, 0, p_canvas_width, p_canvas_height});
}

void VectorServer::cleanup() {
    canvas.reset();
}

// TODO: It might not be a good idea to use view box for clipping.
// Viewport may be a better choice?
void VectorServer::push_scene(const Rect<float> &view_box) {
    canvas->set_size(view_box.size().to_i32());
    scene_stack.push_back(canvas->get_scene());
}

void VectorServer::pop_scene() {
    if (scene_stack.empty()) {
        return;
    }

    scene_stack.erase(scene_stack.end() - 1);

    if (scene_stack.empty()) {
        canvas->set_scene(nullptr);
    } else {
        canvas->set_scene(scene_stack.back());
    }
}

void VectorServer::clear_scene() {
    scene_stack.clear();
    canvas->clear();
    //    push_scene({0, 0, default_canvas_size.x, default_canvas_size.y});
}

void VectorServer::submit() {
    canvas->draw();
}

void VectorServer::draw_line(Vec2F start, Vec2F end, float width, ColorU color) {
    //    if (scene_stack.empty()) {
    //        return;
    //    }
    //
    //    canvas->set_scene(scene_stack.back());

    Pathfinder::Path2d path;
    path.add_line({start.x, start.y}, {end.x, end.y});

    canvas->save_state();

    canvas->set_stroke_paint(Pathfinder::Paint::from_color(color));
    canvas->set_line_width(width);
    // canvas->set_line_cap(Pathfinder::LineCap::Round);
    canvas->stroke_path(path);

    canvas->restore_state();
}

void VectorServer::draw_rectangle(const Rect<float> &rect, float line_width, ColorU color) {
    Pathfinder::Path2d path;
    path.add_rect(rect);

    canvas->save_state();

    canvas->set_stroke_paint(Pathfinder::Paint::from_color(color));
    canvas->set_line_width(line_width);
    canvas->stroke_path(path);

    canvas->restore_state();
}

void VectorServer::draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color) {
    //    if (scene_stack.empty()) {
    //        return;
    //    }
    //
    //    canvas->set_scene(scene_stack.back());

    Pathfinder::Path2d path;
    path.add_circle({center.x, center.y}, radius);

    canvas->save_state();

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

    canvas->set_transform(transform);

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

void VectorServer::draw_texture(VectorTexture &texture, Transform2 transform) {
    for (auto &path : texture.get_paths()) {
        draw_path(path, transform);
    }
}

std::shared_ptr<ImageTexture> VectorServer::get_texture() {
    auto texture_vk = static_cast<Pathfinder::TextureVk *>(canvas->get_dest_texture().get());
    return ImageTexture::from_wrapper(
        texture_vk->get_image_view(), texture_vk->get_sampler(), texture_vk->get_width(), texture_vk->get_height());
}

void VectorServer::set_render_target(std::shared_ptr<ImageTexture> dest_texture) {
}

} // namespace Flint
