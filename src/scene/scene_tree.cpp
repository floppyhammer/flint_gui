#include "scene_tree.h"

namespace Flint {

SceneTree::SceneTree() {
    // TODO: make the root a SubViewport.
    root = std::make_shared<Node>();
    root->name = "main_viewport";
}

std::shared_ptr<Node> SceneTree::get_root() const {
    return root;
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
