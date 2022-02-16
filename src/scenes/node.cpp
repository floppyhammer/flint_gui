#include "node.h"

namespace Flint {
    void Node::update() {
        self_update();

        for (auto& child : children) {
            child.update();
        }
    }

    void Node::draw() {
        self_draw();

        for (auto& child : children) {
            child.draw();
        }
    }

    void Node::self_update() {

    }

    void Node::self_draw() {

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

    std::shared_ptr<Node> Node::get_root() {
        if (parent != nullptr) {
            return parent->get_parent();
        } else {
            return std::shared_ptr<Node>(this);
        }
    }
}