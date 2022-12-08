#include "container.h"

namespace Flint {
Container::Container() {
    // This class is not meant for direct use as a node.
    type = NodeType::Max;
}

void Container::adjust_layout() {
    auto max_size = size.max(minimum_size);
    for (auto &child : children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            cast_child->set_position({0, 0});
            auto child_min_size = cast_child->calc_minimum_size();
            max_size = max_size.max(child_min_size);
        }
    }

    size = max_size;

    for (auto &child : children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            cast_child->set_size(max_size);
        }
    }
}

Vec2F Container::calc_minimum_size() const {
    Vec2F min_size;
    for (auto &child : children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            min_size = cast_child->calc_minimum_size();
        }
    }

    return minimum_size.max(min_size);
}

void Container::set_size(Vec2F p_size) {
    if (size == p_size) {
        return;
    }

    size = p_size;

    adjust_layout();
}

void Container::update(double dt) {
}
} // namespace Flint
