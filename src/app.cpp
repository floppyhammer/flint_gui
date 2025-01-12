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
    Logger::set_default_level(Logger::Level::Silence);
    Logger::set_module_level("Flint", Logger::Level::Warn);

    auto render_server = RenderServer::get_singleton();

    // Create the main window.
    render_server->window_builder_ = Pathfinder::WindowBuilder::new_impl(primary_window_size);
    primary_window_ = render_server->window_builder_->get_primary_window();

    auto input_server = InputServer::get_singleton();
    input_server->initialize_window_callbacks((GLFWwindow*)primary_window_.lock()->get_glfw_handle());

    // Create device and queue.
    render_server->device_ = render_server->window_builder_->request_device();
    render_server->queue_ = render_server->window_builder_->create_queue();

    // Create swap chains for windows.
    primary_swap_chain_ = primary_window_.lock()->get_swap_chain(render_server->device_);

    auto vector_server = VectorServer::get_singleton();
    vector_server->init(primary_window_.lock()->get_physical_size(),
                        render_server->device_,
                        render_server->queue_,
                        Pathfinder::RenderLevel::D3d9);

    tree = std::make_unique<SceneTree>();
    tree->primary_window = primary_window_;

    {
        render_server->blit_ = std::make_shared<Blit>(
            render_server->device_, render_server->queue_, primary_swap_chain_.lock()->get_surface_format());

        vector_target_ = render_server->device_->create_texture(
            {primary_window_.lock()->get_physical_size(), Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
    }
}

App::~App() {
    // Clean up the scene tree.
    tree.reset();

    vector_target_.reset();

    VectorServer::get_singleton()->cleanup();
    Logger::verbose("Cleaned up VectorServer.", "Flint");

    RenderServer::get_singleton()->destroy();
    Logger::verbose("Cleaned up RenderServer.", "Flint");
}

SceneTree* App::get_tree() const {
    return tree.get();
}

std::shared_ptr<Node> App::get_tree_root() const {
    return tree->get_root();
}

void App::set_window_title(const std::string& title) {
    primary_window_.lock()->set_window_title(title);
}

void App::main_loop() {
    while (!primary_window_.lock()->should_close()) {
        InputServer::get_singleton()->clear_events();

        RenderServer::get_singleton()->window_builder_->poll_events();

        if (primary_window_.lock()->get_resize_flag()) {
            vector_target_ = RenderServer::get_singleton()->device_->create_texture(
                {primary_window_.lock()->get_physical_size(), Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
            VectorServer::get_singleton()->get_canvas()->set_size(primary_window_.lock()->get_physical_size());
        }

        // Engine processing.
        Engine::get_singleton()->tick();

        // Get frame time.
        auto dt = Engine::get_singleton()->get_delta();

        // Update the scene tree.
        tree->process(dt);

        // Drawing process for the primary window;
        {
            // Acquire next swap chain image.
            if (!primary_swap_chain_.lock()->acquire_image()) {
                continue;
            }

            auto vector_server = VectorServer::get_singleton();
            vector_server->set_dst_texture(vector_target_);

            auto render_server = RenderServer::get_singleton();
            vector_server->submit_and_clear();

            auto encoder = render_server->device_->create_command_encoder("Main encoder");

            auto surface_texture = primary_swap_chain_.lock()->get_surface_texture();

            // Swap chain render pass.
            {
                encoder->begin_render_pass(
                    primary_swap_chain_.lock()->get_render_pass(), surface_texture, ColorF(0.2, 0.2, 0.2, 1.0));

                encoder->set_viewport({{0, 0}, primary_window_.lock()->get_physical_size()});

                render_server->blit_->set_texture(vector_target_);

                // Draw canvas to screen.
                render_server->blit_->draw(encoder);

                encoder->end_render_pass();
            }

            render_server->queue_->submit(encoder, primary_swap_chain_.lock());

            primary_swap_chain_.lock()->present();
        }
    }

    RenderServer::get_singleton()->window_builder_->stop_and_destroy_swapchains();
}

} // namespace Flint
