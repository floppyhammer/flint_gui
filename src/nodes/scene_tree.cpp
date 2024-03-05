#include "scene_tree.h"

namespace Flint {

SceneTree::SceneTree() {
    // A default root.
    auto node_ui = std::make_shared<NodeUi>();
    node_ui->set_anchor_flag(AnchorFlag::FullRect);

    root = node_ui;
    root->tree_ = this;
}

void SceneTree::replace_scene(const std::shared_ptr<Node>& new_scene) {
    root = new_scene;
    root->tree_ = this;
}

void SceneTree::process(double dt) {
    if (root == nullptr) {
        return;
    }

    if (primary_window && old_primary_window_size != primary_window->get_size()) {
        old_primary_window_size = primary_window->get_size();
        when_primary_window_size_changed(old_primary_window_size);
    }

    for (auto& event : InputServer::get_singleton()->input_queue) {
        root->propagate_input(event);
    }

    root->propagate_update(dt);

    // Collect draw commands.
    root->propagate_draw();

    // Actual drawing.
}

void SceneTree::when_primary_window_size_changed(Vec2I new_size) const {
    root->when_parent_size_changed(new_size.to_f32());
}

std::shared_ptr<Node> SceneTree::get_root() const {
    return root;
}

void SceneTree::quit() {
    quited = true;
}

bool SceneTree::has_quited() const {
    return quited;
}

} // namespace Flint
