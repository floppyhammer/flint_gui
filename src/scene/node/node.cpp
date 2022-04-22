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

    Node *Node::get_viewport() {
        if (get_parent() != nullptr) {
            if (get_parent()->type == NodeType::SubViewport) {
                return get_parent();
            } else {
                return get_parent()->get_viewport();
            }
        } else {
            return nullptr;
        }
    }

    Node *Node::get_parent() {
        return parent;
    }

    std::vector<std::shared_ptr<Node>> Node::get_children() {
        return children;
    }

    void Node::add_child(const std::shared_ptr<Node> &p_child) {
        // Set self as parent of the new node.
        p_child->parent = this;

        children.push_back(p_child);
    }

    void Node::remove_child(size_t index) {
        if (index < 0 || index >= children.size()) return;
        children.erase(children.begin() + index);
    }

    NodeType Node::extended_from_which_base_node() const {
        if (type < NodeType::NodeGui)
            return NodeType::Node;
        else if (type < NodeType::Node2D)
            return NodeType::NodeGui;
        else if (type < NodeType::Node3D)
            return NodeType::Node2D;
        else if (type < NodeType::Max)
            return NodeType::Node3D;
        else
            return NodeType::Max;
    }

    void Node::_cleanup() {
        for (auto &child: children) {
            child->_cleanup();
        }
    }

    void Node::update(double delta) {
    }

    void Node::draw(VkCommandBuffer p_command_buffer) {
    }
}
