#include "scene_tree.h"

#include "../rendering/rendering_server.h"

#include <array>

namespace Flint {
    void SceneTree::update_tree() {
        // Check tree status. Record commands if necessary.
//        if (tree_changed) {
//            record_commands();
//        }

        root->update(0.001);
    }

    void SceneTree::record_commands(VkCommandBuffer command_buffer) const {
        // Every time tree structure changes, we need to rebuild the persistent command queue.
        root->draw();
    }
}
