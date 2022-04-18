#ifndef FLINT_NODE_H
#define FLINT_NODE_H

#include "../../core/engine.h"
#include "../../common/logger.h"

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
        NodeGui,
        TextureRect,
        SubViewportContainer,

        // 2D.
        Node2D,
        Sprite2D,
        RigidBody2D,

        // 3D.
        Node3D,
        Sprite3D,
        Model,

        Max,
    };

    class Node {
    public:
        NodeType type = NodeType::Node;

        virtual void _update(double delta);

        virtual void _draw(VkCommandBuffer p_command_buffer);

        virtual void _notify(Signal signal);

        void _cleanup();

        virtual void update(double delta);

        virtual void draw(VkCommandBuffer p_command_buffer);

        void add_child(const std::shared_ptr<Node> &p_child);

        /**
         * Get the viewport this node belongs to.
         * @return A pointer to the viewport.
         */
        virtual Node *get_viewport();

        Node *get_parent();

        std::vector<std::shared_ptr<Node>> get_children();

        void remove_child(size_t index);

        NodeType extended_from_which_base_node() const;

    protected:
        std::vector<std::shared_ptr<Node>> children;

        // Don't use a shared pointer as it causes circular references.
        Node *parent;
    };
}

#endif //FLINT_NODE_H
