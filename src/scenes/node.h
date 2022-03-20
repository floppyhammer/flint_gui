#ifndef FLINT_NODE_H
#define FLINT_NODE_H

#include "../core/engine.h"
#include "../common/logger.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <vector>
#include <memory>

namespace Flint {
    enum class NodeType {
        // General.
        Node = 0,
        SubViewport,

        // GUI.
        Control,
        TextureRect,
        SubViewportContainer,

        // 2D.
        Node2D,

        // 3D.
        Node3D,
        MeshInstance3D,

        Max,
    };

    class Node {
    public:
        NodeType type = NodeType::Node;

        virtual void update(double delta);

        virtual void draw(VkCommandBuffer p_command_buffer);

        virtual void notify(Signal signal);

        void add_child(const std::shared_ptr<Node> &p_child);

        /**
         * Get the viewport this node belongs to.
         * @return A pointer to the viewport.
         */
        virtual Node *get_viewport();

        Node *get_parent();

        std::vector<std::shared_ptr<Node>> get_children();

        void remove_child(size_t index);

        void cleanup();

    protected:
        std::vector<std::shared_ptr<Node>> children;

        // Don't use a shared pointer as it causes circular references.
        Node *parent;
    };
}

#endif //FLINT_NODE_H
