#include "scene_tree.h"

#include <pathfinder-cpp/include/pathfinder.h>

#include "render/swap_chain.h"

namespace Flint {

SceneTree::SceneTree(Vec2I main_window_size) {
    auto display_server = DisplayServer::get_singleton();

    root = std::make_shared<WindowProxy>(main_window_size, false);
    root->name = "Main Window";

    // Initialize the render server after creating the first window (surface).
    auto render_server = RenderServer::get_singleton();

    // Initialize the vector server.
    auto device = std::make_shared<Pathfinder::DeviceVk>(display_server->get_device(),
                                                         display_server->physicalDevice,
                                                         display_server->graphicsQueue,
                                                         display_server->command_pool);
    auto queue = std::make_shared<Pathfinder::QueueVk>(
        display_server->get_device(), display_server->graphicsQueue, display_server->graphicsQueue);
    auto vector_server = VectorServer::get_singleton();
    vector_server->init(main_window_size, device, queue, Pathfinder::RenderLevel::Dx9);
}

void SceneTree::replace_scene(const std::shared_ptr<Node>& new_scene) {
    switch (new_scene->extended_from_which_base_node()) {
        case NodeType::Node:
        case NodeType::Node3d: {
            auto world = std::make_shared<World>(false);
            world->add_child(new_scene);
            root->add_child(world);
        } break;
        case NodeType::Node2d: {
            auto world = std::make_shared<World>(true);
            world->add_child(new_scene);
            root->add_child(world);
        } break;
        case NodeType::NodeUi: {
            auto ui_layer = std::make_shared<UiLayer>();
            ui_layer->add_child(new_scene);
            root->add_child(ui_layer);
        } break;
        default:
            abort();
    }
}

void SceneTree::process(double dt) const {
    if (root == nullptr) {
        return;
    }

    for (auto& event : InputServer::get_singleton()->input_queue) {
        root->propagate_input(event);
    }

    root->propagate_update(dt);

    root->propagate_draw(VK_NULL_HANDLE, VK_NULL_HANDLE);
}

void SceneTree::when_window_size_changed(Vec2I new_size) const {
    for (auto& child : root->get_children()) {
        if (child->get_node_type() == NodeType::UiLayer) {
            auto cast_child = dynamic_cast<UiLayer*>(child.get());
            cast_child->when_window_size_changed(new_size);
        }
    }
}

void SceneTree::quit() {
    quited = true;
}

bool SceneTree::has_quited() const {
    return quited;
}

} // namespace Flint
