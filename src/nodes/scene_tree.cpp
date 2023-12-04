#include "scene_tree.h"

namespace Flint {

SceneTree::SceneTree() {
    // A default root.
    root = std::make_shared<Node>();
    root->tree_ = this;
}

void SceneTree::replace_scene(const std::shared_ptr<Node>& new_scene) {
    root = new_scene;
    root->tree_ = this;
}

void SceneTree::process(double dt) const {
    if (root == nullptr) {
        return;
    }

    for (auto& event : InputServer::get_singleton()->input_queue) {
        root->propagate_input(event);
    }

    root->propagate_update(dt);

    root->propagate_draw();
}

void SceneTree::when_window_size_changed(Vec2I new_size) const {
    for (auto& child : root->get_children()) {
        // if (child->get_node_type() == NodeType::UiLayer) {
        //     auto cast_child = dynamic_cast<UiLayer*>(child.get());
        //     cast_child->when_window_size_changed(new_size);
        // }
    }
}

void SceneTree::quit() {
    quited = true;
}

bool SceneTree::has_quited() const {
    return quited;
}

} // namespace Flint
