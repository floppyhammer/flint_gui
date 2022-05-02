#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "node.h"

namespace Flint {
    class SceneTree {
    public:
        SceneTree() = default;

        void input(std::vector<InputEvent> &input_queue) const;

        void update(double dt) const;

        void draw(VkCommandBuffer p_command_buffer) const;

        void set_root(std::shared_ptr<Node>);

        std::shared_ptr<Node> get_root() const;

    private:
        std::shared_ptr<Node> root;
    };
}

#endif //FLINT_SCENE_TREE_H
