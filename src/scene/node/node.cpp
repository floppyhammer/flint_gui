#include "node.h"

namespace Flint {
    void Node::_update(double dt) {
        for (auto &child: children) {
            child->_update(dt);
        }
    }

    void Node::_draw(VkCommandBuffer p_command_buffer) {
        for (auto &child: children) {
            child->_draw(p_command_buffer);
        }
    }

    void Node::_notify(Signal signal) {
        for (auto &child: children) {
            child->_notify(signal);
        }
    }

    void Node::propagate_input(std::vector<InputEvent> &input_queue) {
        for (auto &child: children) {
            child->propagate_input(input_queue);
        }
    }

    void Node::propagate_cleanup() {
        for (auto &child: children) {
            child->propagate_cleanup();
        }
    }

    Node *Node::get_viewport() {
        if (get_parent() == nullptr) return nullptr;

        return get_parent()->type == NodeType::SubViewport ? get_parent() : get_parent()->get_viewport();
    }

    Node *Node::get_parent() {
        return parent;
    }

    std::vector<std::shared_ptr<Node>> Node::get_children() {
        return children;
    }

    void Node::add_child(const std::shared_ptr<Node> &p_child) {
        // Set self as the parent of the new node.
        p_child->parent = this;

        children.push_back(p_child);
    }

    void Node::remove_child(size_t index) {
        if (index < 0 || index >= children.size()) return;
        children.erase(children.begin() + index);
    }

    NodeType Node::extended_from_which_base_node() const {
        if (type < NodeType::Control)
            return NodeType::Node;
        else if (type < NodeType::Node2D)
            return NodeType::Control;
        else if (type < NodeType::Node3D)
            return NodeType::Node2D;
        else if (type < NodeType::Max)
            return NodeType::Node3D;
        else
            return NodeType::Max;
    }

    void Node::update(double delta) {
    }

    void Node::draw(VkCommandBuffer p_command_buffer) {
    }
}
