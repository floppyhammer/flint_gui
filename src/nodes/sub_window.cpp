#include "sub_window.h"

#include "../common/geometry.h"
#include "../servers/render_server.h"
#include "../servers/vector_server.h"

namespace Flint {

SubWindow::SubWindow(Vec2I size) {
    type = NodeType::Window;

    size_ = size;

    auto render_server = RenderServer::get_singleton();
    window_index_ = render_server->window_builder_->create_window(size_, "SubWindow");

    auto window = render_server->window_builder_->get_window(window_index_).lock();

    auto input_server = InputServer::get_singleton();
    input_server->initialize_window_callbacks(window_index_);

    auto swap_chain_ = window->get_swap_chain(render_server->device_);

    vector_target_ =
        render_server->device_->create_texture({size_, Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
}

Vec2I SubWindow::get_size() const {
    return size_;
}

void SubWindow::update(double dt) {
    // if (window_->get_resize_flag()) {
    //     // auto new_size = Vec2I(window_->get_size(), window->framebuffer_height);
    //     //        when_window_size_changed(new_size);
    //     //        VectorServer::get_singleton()->get_canvas()->set_new_dst_texture(new_size);
    // }

    auto render_server = RenderServer::get_singleton();
    auto window = render_server->window_builder_->get_window(window_index_).lock();

    // Closing a window just hides it.
    if (window->should_close() || !visible_) {
        window->hide();
    } else {
        window->show();
    }
}

void SubWindow::pre_draw_children() {
    if (!visible_) {
        return;
    }

    auto render_server = RenderServer::get_singleton();
    auto window = render_server->window_builder_->get_window(window_index_).lock();
    auto swap_chain_ = window->get_swap_chain(render_server->device_);

    // Acquire next swap chain image.
    if (!swap_chain_->acquire_image()) {
        return;
    }

    auto vector_server = VectorServer::get_singleton();

    if (window->get_resize_flag()) {
        vector_target_ = render_server->device_->create_texture(
            {window->get_logical_size(), Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
    }

    vector_server->set_dst_texture(vector_target_);

    temp_draw_data.previous_scene = vector_server->get_canvas()->take_scene();

    vector_server->get_canvas()->set_size(window->get_logical_size());
}

void SubWindow::post_draw_children() {
    auto render_server = RenderServer::get_singleton();
    auto vector_server = VectorServer::get_singleton();

    auto window = render_server->window_builder_->get_window(window_index_).lock();
    auto swap_chain_ = window->get_swap_chain(render_server->device_);

    vector_server->submit_and_clear();

    vector_server->get_canvas()->set_scene(temp_draw_data.previous_scene);

    auto encoder = render_server->device_->create_command_encoder("Main encoder");

    auto surface_texture = swap_chain_->get_surface_texture();

    // Swap chain render pass.
    {
        encoder->begin_render_pass(swap_chain_->get_render_pass(), surface_texture, ColorF(0.2, 0.2, 0.2, 1.0));

        encoder->set_viewport({{0, 0}, window->get_logical_size()});

        render_server->blit_->set_texture(vector_target_);

        // Draw canvas to screen.
        render_server->blit_->draw(encoder);

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

std::shared_ptr<Pathfinder::Window> SubWindow::get_raw_window() const {
    auto render_server = RenderServer::get_singleton();

    auto window = render_server->window_builder_->get_window(window_index_).lock();

    return window;
}

} // namespace Flint
