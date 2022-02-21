#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "../scenes/node.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

namespace Flint {
    class SceneTree {
    public:
        std::shared_ptr<Node> root;

        void update_tree();

        void record_commands(VkCommandBuffer command_buffer) const;

    private:
        bool tree_changed = false;
    };
}

#endif //FLINT_SCENE_TREE_H
