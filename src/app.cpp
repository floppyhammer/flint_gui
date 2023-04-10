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
#include "servers/display_server.h"
#include "servers/engine.h"
#include "servers/input_server.h"
#include "servers/render_server.h"
#include "servers/vector_server.h"

using namespace Flint;

App::App(Vec2I window_size) {
    // Set logger level.
    Utils::Logger::set_level(Utils::Logger::Level::VERBOSE);

    tree = std::make_unique<Scene::SceneTree>(window_size);
}

App::~App() {
    cleanup();
}

Scene::SceneTree* App::get_tree() {
    return tree.get();
}

void App::main_loop() {
    while (!tree->has_quited()) {
        // Collect input and window events.
        InputServer::get_singleton()->collect_events();

        // Engine processing.
        Engine::get_singleton()->tick();

        // Get frame time.
        auto dt = Engine::get_singleton()->get_delta();

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
    Utils::Logger::verbose("Cleaned up VectorServer.", "App");

    DefaultResource::get_singleton()->cleanup();
    Utils::Logger::verbose("Cleaned up DefaultResource.", "App");

    RenderServer::get_singleton()->cleanup();
    Utils::Logger::verbose("Cleaned up RenderServer.", "App");

    DisplayServer::get_singleton()->cleanup();
    Utils::Logger::verbose("Cleaned up DisplayServer.", "App");
}
