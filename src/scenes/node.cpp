#include "node.h"

namespace Flint {
    void Node::update(double delta) {
        for (auto& child : children) {
            child->update(delta);
        }
    }

    void Node::draw() {
        for (auto& child : children) {
            child->draw();
        }
    }

    void Node::notify(Signal signal) {
        for (auto& child : children) {
            notify(signal);
        }
    }

    std::shared_ptr<SubViewport> Node::get_viewport() {
        if (get_parent() != nullptr) {
            return get_parent()->get_viewport();
        } else {
            return {};
        }
    }

    std::shared_ptr<Node> Node::get_parent() {
        return parent;
    }

    std::vector<std::shared_ptr<Node>> Node::get_children() {
        return children;
    }

    void Node::add_child(const std::shared_ptr<Node>& p_child) {
        // Set self as parent of the new node.
        p_child->parent = std::shared_ptr<Node>(this);

        children.push_back(p_child);
    }

    void Node::remove_child(size_t index) {
        if (index < 0 || index >= children.size()) return;
        children.erase(children.begin() + index);
    }
}
