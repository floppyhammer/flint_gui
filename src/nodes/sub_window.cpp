#include "sub_window.h"

#include "../common/geometry.h"
#include "../servers/render_server.h"
#include "../servers/vector_server.h"

namespace Flint {

SubWindow::SubWindow(Vec2I size) {
    type = NodeType::Window;

    size_ = size;

    auto render_server = RenderServer::get_singleton();
    window_ = render_server->window_builder_->create_window(size_, "SubWindow");

    auto input_server = InputServer::get_singleton();
    input_server->initialize_window_callbacks((GLFWwindow *)window_->get_glfw_handle());

    swap_chain_ = window_->get_swap_chain(render_server->device_);

    vector_target_ =
        render_server->device_->create_texture({size_, Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
}

Vec2I SubWindow::get_size() const {
    return size_;
}

void SubWindow::propagate_input(InputEvent &event) {
    if (!visible_) {
        return;
    }

    // Filter events not belonging to this window.
    if (event.window != window_->get_glfw_handle()) {
        return;
    }

    auto it = children.rbegin();
    while (it != children.rend()) {
        (*it)->propagate_input(event);
        ++it;
    }

    input(event);
}

void SubWindow::propagate_update(double dt) {
    // if (window_->get_resize_flag()) {
    //     // auto new_size = Vec2I(window_->get_size(), window->framebuffer_height);
    //     //        when_window_size_changed(new_size);
    //     //        VectorServer::get_singleton()->get_canvas()->set_new_dst_texture(new_size);
    // }

    // Closing a window just hides it.
    if (window_->should_close() || !visible_) {
        window_->hide();
    } else {
        window_->show();
    }

    if (!visible_) {
        return;
    }

    for (auto &child : children) {
        child->propagate_update(dt);
    }
}

void SubWindow::propagate_draw() {
    if (!visible_) {
        return;
    }

    // Acquire next swap chain image.
    if (!swap_chain_->acquire_image()) {
        return;
    }

    auto render_server = RenderServer::get_singleton();

    auto vector_server = VectorServer::get_singleton();

    if (window_->get_resize_flag()) {
        vector_target_ = render_server->device_->create_texture(
            {window_->get_size(), Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
    }

    vector_server->set_dst_texture(vector_target_);

    auto previous_scene = vector_server->get_canvas()->take_scene();

    vector_server->get_canvas()->set_size(window_->get_size());

    for (auto &child : children) {
        child->propagate_draw();
    }

    vector_server->submit_and_clear();

    vector_server->get_canvas()->set_scene(previous_scene);

    auto encoder = render_server->device_->create_command_encoder("Main encoder");

    auto surface_texture = swap_chain_->get_surface_texture();

    // Swap chain render pass.
    {
        encoder->begin_render_pass(swap_chain_->get_render_pass(), surface_texture, ColorF(0.2, 0.2, 0.2, 1.0));

        encoder->set_viewport({{0, 0}, window_->get_size()});

        render_server->blit_->set_texture(vector_target_);

        // Draw canvas to screen.
        render_server->blit_->draw(encoder, window_->get_size());

        encoder->end_render_pass();
    }

    render_server->queue_->submit(encoder, swap_chain_);

    swap_chain_->present();
}

void SubWindow::set_visibility(bool visible) {
    if (visible_ == visible) {
        return;
    }

    visible_ = visible;
}

} // namespace Flint
