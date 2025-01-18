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
    render_server->window_builder_ = Pathfinder::WindowBuilder::new_impl(primary_window_size, false);
    auto primary_window = render_server->window_builder_->get_window(0);

    auto input_server = InputServer::get_singleton();
    input_server->initialize_window_callbacks(0);

    // Create device and queue.
    render_server->device_ = render_server->window_builder_->request_device();
    render_server->queue_ = render_server->window_builder_->create_queue();

    // Create swap chains for windows.
    auto primary_swap_chain = primary_window.lock()->get_swap_chain(render_server->device_);

    auto vector_server = VectorServer::get_singleton();
    vector_server->init(primary_window.lock()->get_physical_size(),
                        render_server->device_,
                        render_server->queue_,
                        Pathfinder::RenderLevel::D3d9);

    tree = std::make_unique<SceneTree>();

    {
        render_server->blit_ = std::make_shared<Blit>(
            render_server->device_, render_server->queue_, primary_swap_chain->get_surface_format());

        vector_target_ = render_server->device_->create_texture(
            {primary_window.lock()->get_physical_size(), Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
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
    auto render_server = RenderServer::get_singleton();
    auto primary_window = render_server->window_builder_->get_window(0);
    primary_window.lock()->set_window_title(title);
}

void App::set_fullscreen(bool fullscreen) {
    auto render_server = RenderServer::get_singleton();

    render_server->window_builder_->set_fullscreen(fullscreen);
}

std::shared_ptr<Pathfinder::Window> get_primary_window() {
    auto render_server = RenderServer::get_singleton();

    return render_server->window_builder_->get_window(0).lock();
}

void App::main_loop() {
    auto render_server = RenderServer::get_singleton();

    while (!get_primary_window()->should_close()) {
        InputServer::get_singleton()->clear_events();
        RenderServer::get_singleton()->window_builder_->poll_events();

        auto primary_window = get_primary_window();

        if (primary_window->get_physical_size() != vector_target_->get_size()) {
            if (!primary_window->get_physical_size().is_any_zero()) {
                std::ostringstream ss;
                ss << "Vector target of the primary window resized to " << vector_target_->get_size();
                Logger::info(ss.str(), "Flint");

                vector_target_ = RenderServer::get_singleton()->device_->create_texture(
                    {primary_window->get_physical_size(), Pathfinder::TextureFormat::Rgba8Unorm}, "dst texture");
                VectorServer::get_singleton()->set_canvas_size(primary_window->get_physical_size());
            }
        }

        // Engine processing.
        Engine::get_singleton()->tick();

        // Get frame time.
        auto dt = Engine::get_singleton()->get_delta();

        // Update the scene tree.
        tree->process(dt);

        auto primary_swap_chain = primary_window->get_swap_chain(render_server->device_);

        // Drawing process for the primary window;
        {
            // Acquire next swap chain image.
            if (!primary_swap_chain->acquire_image()) {
                continue;
            }

            auto vector_server = VectorServer::get_singleton();
            vector_server->set_dst_texture(vector_target_);

            auto render_server = RenderServer::get_singleton();
            vector_server->submit_and_clear();

            auto encoder = render_server->device_->create_command_encoder("Main encoder");

            auto surface_texture = primary_swap_chain->get_surface_texture();

            // Swap chain render pass.
            {
                encoder->begin_render_pass(
                    primary_swap_chain->get_render_pass(), surface_texture, ColorF(0.2, 0.2, 0.2, 1.0));

                encoder->set_viewport({{0, 0}, primary_window->get_physical_size()});

                render_server->blit_->set_texture(vector_target_);

                // Draw canvas to screen.
                render_server->blit_->draw(encoder);

                encoder->end_render_pass();
            }

            render_server->queue_->submit(encoder, primary_swap_chain);

            primary_swap_chain->present();
        }
    }

    RenderServer::get_singleton()->window_builder_->stop_and_destroy_swapchains();
}

} // namespace Flint
