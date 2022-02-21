#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "../scenes/node.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

namespace Flint {
    class SceneTree {
    public:
        SceneTree() = default;

        void update_tree(double delta) const;

        void record_commands() const;

        void set_root(std::shared_ptr<Node>);
        std::shared_ptr<Node> get_root();

    private:
        bool tree_changed = false;

        std::shared_ptr<Node> root;
    };
}

#endif //FLINT_SCENE_TREE_H
