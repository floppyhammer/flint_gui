#include "scene_tree.h"

#include "ui_layer.h"
#include "window.h"

namespace Flint {

SceneTree::SceneTree() {
    root = std::make_shared<Window>();
    root->name = "Main Window";
}

void SceneTree::replace_scene(std::shared_ptr<Node> new_scene) {
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

void SceneTree::input(std::vector<InputEvent>& input_queue) const {
    if (root == nullptr || input_queue.empty()) {
        return;
    }

    for (auto& event : input_queue) {
        root->propagate_input(event);
    }
}

void SceneTree::update(double dt) const {
    if (root == nullptr) {
        return;
    }

    root->propagate_update(dt);
}

void SceneTree::draw(VkCommandBuffer cmd_buffer) const {
    if (root == nullptr) {
        return;
    }

    root->propagate_draw(cmd_buffer);
}

} // namespace Flint
