#include "app.h"

#include <cstdint>
#include <memory>

#include "common/load_file.h"
#include "resources/default_resource.h"
#include "resources/resource_manager.h"
#include "servers/engine.h"
#include "servers/input_server.h"
#include "servers/render_server.h"
#include "servers/vector_server.h"

namespace Flint {

App::App(Vec2I primary_window_size) {
    // Set logger level.
    Logger::set_level(Logger::Level::Verbose);

    auto render_server = RenderServer::get_singleton();

    // Create the main window.
    render_server->window_builder_ = Pathfinder::WindowBuilder::new_impl(primary_window_size);
    primary_window_ = render_server->window_builder_->get_primary_window();

    auto input_server = InputServer::get_singleton();
    input_server->initialize_window_callbacks((GLFWwindow*)primary_window_->get_raw_handle());

    // Create device and queue.
    render_server->device_ = render_server->window_builder_->request_device();
    render_server->queue_ = render_server->window_builder_->create_queue();

    // Create swap chains for windows.
    primary_swap_chain_ = primary_window_->get_swap_chain(render_server->device_);

    auto vector_server = VectorServer::get_singleton();
    vector_server->init(
        primary_window_->get_size(), render_server->device_, render_server->queue_, Pathfinder::RenderLevel::Dx9);

    tree = std::make_unique<SceneTree>();
    tree->primary_window = primary_window_;

    {
        render_server->blit_ = std::make_shared<Blit>(
            render_server->device_, render_server->queue_, primary_swap_chain_->get_render_pass());

        vector_target_ = render_server->device_->create_texture(
            {primary_window_size, Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
    }
}

App::~App() {
    // Clean up the scene tree.
    tree.reset();

    VectorServer::get_singleton()->cleanup();
    Logger::verbose("Cleaned up VectorServer.", "App");
}

SceneTree* App::get_tree() {
    return tree.get();
}

void App::main_loop() {
    while (!primary_window_->should_close()) {
        InputServer::get_singleton()->clear_events();

        primary_window_->poll_events();

        if (primary_window_->get_resize_flag()) {
            vector_target_ = RenderServer::get_singleton()->device_->create_texture(
                {primary_window_->get_size(), Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
            VectorServer::get_singleton()->get_canvas()->set_size(primary_window_->get_size());
        }

        // Acquire next swap chain image.
        if (!primary_swap_chain_->acquire_image()) {
            continue;
        }

        // Engine processing.
        Engine::get_singleton()->tick();

        // Get frame time.
        auto dt = Engine::get_singleton()->get_delta();

        auto vector_server = VectorServer::get_singleton();
        vector_server->set_dst_texture(vector_target_);

        auto render_server = RenderServer::get_singleton();

        // Update the scene tree.
        tree->process(dt);

        vector_server->submit_and_clear();

        auto encoder = render_server->device_->create_command_encoder("Main encoder");

        auto framebuffer = primary_swap_chain_->get_framebuffer();

        // Swap chain render pass.
        {
            encoder->begin_render_pass(primary_swap_chain_->get_render_pass(), framebuffer, ColorF(0.2, 0.2, 0.2, 1.0));

            render_server->blit_->set_texture(vector_target_);

            // Draw canvas to screen.
            render_server->blit_->draw(encoder, framebuffer->get_size());

            encoder->end_render_pass();
        }

        render_server->queue_->submit(encoder, primary_swap_chain_);

        primary_swap_chain_->present();
    }
}

} // namespace Flint
