#ifndef FLINT_NODE_H
#define FLINT_NODE_H

#include "../../core/engine.h"
#include "../../common/logger.h"
#include "../../servers/input_server.h"
#include "../../render/render_server.h"

#include <vector>
#include <memory>

namespace Flint {
    enum class NodeType {
        // General.
        Node = 0,
        SubViewport,
        CanvasLayer,

        // GUI.
        Control,
        Container,
        CenterContainer,
        MarginContainer,
        BoxContainer,
        SubViewportContainer,
        Button,
        ItemList,
        Label,
        TextEdit,
        Panel,
        TextureRect,
        Tree,
        ProgressBar,

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

    const std::array<std::string, 24> NodeTypeName {
        // General.
        "Node",
        "SubViewport",
        "CanvasLayer",

        // GUI.
        "Control",
        "Container",
        "CenterContainer",
        "MarginContainer",
        "BoxContainer",
        "SubViewportContainer",
        "Button",
        "ItemList",
        "Label",
        "TextEdit",
        "Panel",
        "TextureRect",
        "Tree",
        "ProgressBar",

        // 2D.
        "Node2D",
        "Sprite2D",
        "RigidBody2D",

        // 3D.
        "Node3D",
        "Sprite3D",
        "Model",
    };

    class Node {
    public:
        NodeType type = NodeType::Node;

        std::string name;

        virtual void propagate_update(double delta);

        virtual void propagate_notify(Signal signal);

        virtual void propagate_input(std::vector<InputEvent> &input_queue);

        virtual void propagate_draw(VkCommandBuffer p_command_buffer);

        virtual void propagate_cleanup();

        virtual void update(double delta);

        virtual void notify(Signal signal);

        virtual void input(std::vector<InputEvent> &input_queue);

        virtual void draw(VkCommandBuffer p_command_buffer);

        void add_child(const std::shared_ptr<Node> &p_child);

        /**
         * Get the viewport this node belongs to.
         * @return A pointer to the viewport.
         */
        virtual Node *get_viewport();

        Node *get_parent();

        void set_parent(Node *node);

        std::vector<std::shared_ptr<Node>> get_children();

        void remove_child(size_t index);

        NodeType extended_from_which_base_node() const;

        std::string get_node_path() const;

    protected:
        std::vector<std::shared_ptr<Node>> children;

        // Don't use a shared pointer as it causes circular references.
        // Also, we must initialize it.
        Node *parent{};
    };
}

#endif //FLINT_NODE_H
