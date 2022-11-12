#include "scene_tree.h"

#include <array>
#include <utility>

namespace Flint {

SceneTree::SceneTree() {
    root = std::make_shared<Node>();
    root->name = "root";
}

std::shared_ptr<Node> SceneTree::get_root() const {
    return root;
}

void SceneTree::update(double dt) const {
    if (root == nullptr) return;

    root->propagate_update(dt);
}

void SceneTree::draw(VkCommandBuffer p_command_buffer) const {
    if (root == nullptr) return;

    root->propagate_draw(p_command_buffer);
}

void SceneTree::input(std::vector<InputEvent>& input_queue) const {
    if (root == nullptr || input_queue.empty()) {
        return;
    }

    for (auto& event : InputServer::get_singleton()->input_queue) {
        root->propagate_input(event);
    }
}

} // namespace Flint
