#include "container.h"

namespace revector {

Container::Container() {
    type = NodeType::NotInstantiable;
}

void Container::adjust_layout() {
    // Get the minimum size.
    auto min_size = get_effective_minimum_size();

    // Adjust own size.
    size = size.max(min_size);

    // Adjust child size.
    for (auto &child : children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            cast_child->set_position({0, 0});
            cast_child->set_size(size);
        }
    }
}

void Container::calc_minimum_size() {
    // Get the minimum child size.
    Vec2F min_child_size{};
    for (const auto &child : children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            auto child_min_size = cast_child->get_effective_minimum_size();
            min_child_size = min_child_size.max(child_min_size);
        }
    }

    calculated_minimum_size = min_child_size;
}

void Container::update(double dt) {
    NodeUi::update(dt);

    adjust_layout();
}

std::vector<NodeUi *> Container::get_visible_ui_children() const {
    // Get UI children.
    std::vector<NodeUi *> ui_children;
    ui_children.reserve(children.size());

    for (const auto &child : children) {
        // We only care about visible GUI nodes in a container.
        if (!child->get_visibility() || !child->is_ui_node()) {
            continue;
        }

        auto cast_child = dynamic_cast<NodeUi *>(child.get());

        ui_children.push_back(cast_child);
    }

    return ui_children;
}

} // namespace revector
