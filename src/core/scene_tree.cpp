#include "scene_tree.h"

#include "../rendering/rendering_server.h"

#include <array>
#include <utility>

namespace Flint {
    void SceneTree::set_root(std::shared_ptr<Node> p_node) {
        root = std::move(p_node);
    }

    std::shared_ptr<Node> SceneTree::get_root() {
        return root;
    }

    void SceneTree::record_commands(VkCommandBuffer p_command_buffer) const {
        if (root == nullptr) return;
        root->draw(p_command_buffer);
    }

    void SceneTree::update(double delta) const {
        if (root == nullptr) return;
        root->update(delta);
    }
}
