#include "scene_tree.h"

#include <array>
#include <utility>

namespace Flint {
    void SceneTree::set_root(std::shared_ptr<Node> p_node) {
        root = std::move(p_node);
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

    void SceneTree::input(std::vector<InputEvent> &input_queue) const {
        if (root == nullptr) return;
        root->propagate_input(input_queue);
    }
}
