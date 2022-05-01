#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "../scene/node/node.h"
#include "../servers/input_server.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

namespace Flint {
    class SceneTree {
    public:
        SceneTree() = default;

        void input(const InputEvent &input_event) const;

        void update(double dt) const;

        void draw(VkCommandBuffer p_command_buffer) const;

        void set_root(std::shared_ptr<Node>);

        [[nodiscard]] std::shared_ptr<Node> get_root() const;

    private:
        std::shared_ptr<Node> root;
    };
}

#endif //FLINT_SCENE_TREE_H
