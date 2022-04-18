#include "scene_tree.h"

#include "../rendering/render_server.h"

#include <array>
#include <utility>

namespace Flint {
    void SceneTree::set_root(std::shared_ptr<Node> p_node) {
        root = std::move(p_node);
    }

    std::shared_ptr<Node> SceneTree::get_root() const {
        return root;
    }

    void SceneTree::update(double delta) const {
        if (root == nullptr) return;
        root->_update(delta);
    }

    void SceneTree::draw(VkCommandBuffer p_command_buffer) const {
        if (root == nullptr) return;
        root->_draw(p_command_buffer);
    }

    void SceneTree::input(const InputEvent &input_event) const {

    }
}
