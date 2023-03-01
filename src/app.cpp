#include "app.h"

#include <cstdint>
#include <memory>

#include "common/load_file.h"
#include "common/obj_importer.h"
#include "render/swap_chain.h"
#include "render/window.h"
#include "resources/default_resource.h"
#include "resources/image_texture.h"
#include "resources/mesh.h"
#include "resources/resource_manager.h"
#include "servers/core_server.h"
#include "servers/display_server.h"
#include "servers/input_server.h"
#include "servers/render_server.h"
#include "servers/vector_server.h"

using namespace Flint;

App::App(int32_t window_width, int32_t window_height) {
    // Set logger level.
    Logger::set_level(Logger::VERBOSE);

    auto display_server = DisplayServer::get_singleton();

    //    // 3. Initialize swap chain.
    //    auto swap_chain = SwapChain::get_singleton();
    //
    //    // 4. Initialize input server.
    //    auto input_server = InputServer::get_singleton();
    //    input_server->init(window->glfw_window);

    tree = std::make_unique<Flint::SceneTree>(Vec2I{window_width, window_height});
}

App::~App() {
    cleanup();
}

SceneTree* App::get_tree() {
    return tree.get();
}

void App::main_loop() {
    while (!tree->has_quited()) {
        //        // Collect input and window events.
        //        InputServer::get_singleton()->collect_events();
        glfwPollEvents();

        // Engine processing.
        CoreServer::get_singleton()->tick();

        // Get frame time.
        auto dt = CoreServer::get_singleton()->get_delta();

        // Propagate input events.
        //        tree->input(InputServer::get_singleton()->input_queue);

        // Update the scene tree.
        tree->process(dt);

        //        // Record draw calls.
        //        record_commands(SwapChain::get_singleton()->commandBuffers, image_index);
        //
        //        // Submit commands for drawing.
        //        SwapChain::get_singleton()->flush(image_index);
    }

    cleanup();
}

void App::cleanup() {
    // Clean up the scene tree.
    tree.reset();

    VectorServer::get_singleton()->cleanup();
    Logger::verbose("Cleaned up VectorServer.", "App");

    DefaultResource::get_singleton()->cleanup();
    Logger::verbose("Cleaned up DefaultResource.", "App");

    RenderServer::get_singleton()->cleanup();
    Logger::verbose("Cleaned up RenderServer.", "App");

    DisplayServer::get_singleton()->cleanup();
    Logger::verbose("Cleaned up DisplayServer.", "App");
}
