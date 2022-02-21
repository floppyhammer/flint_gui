#include "scene_tree.h"

#include "../rendering/rendering_server.h"

#include <array>
#include <utility>

namespace Flint {
    void SceneTree::update_tree(double delta) const {
        // Check tree status. Record commands if necessary.
//        if (tree_changed) {
//            record_commands();
//        }

        root->update(delta);
    }

    void SceneTree::record_commands() const {
        // Every time tree structure changes, we need to rebuild the persistent command queue.
        root->draw();
    }

    void SceneTree::set_root(std::shared_ptr<Node> p_node) {
        root = std::move(p_node);
    }

    std::shared_ptr<Node> SceneTree::get_root() {
        return root;
    }
}
