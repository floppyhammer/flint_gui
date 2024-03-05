#include "container.h"

namespace Flint {

Container::Container() {
    type = NodeType::Container;

    debug_size_box.border_color = ColorU::white();
}

void Container::adjust_layout() {
    // Get the minimum size.
    auto min_size = calc_minimum_size();

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

Vec2F Container::calc_minimum_size() const {
    // Get the minimum child size.
    Vec2F min_child_size{};
    for (const auto &child : children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            auto child_min_size = cast_child->calc_minimum_size();
            min_child_size = min_child_size.max(child_min_size);
        }
    }

    // Take own minimum size into account.
    return minimum_size.max(min_child_size);
}

void Container::set_size(Vec2F new_size) {
    if (size == new_size) {
        return;
    }

    auto min_size = calc_minimum_size();

    size = new_size.max(min_size);
}

void Container::update(double dt) {
    NodeUi::update(dt);

    adjust_layout();
}

} // namespace Flint
