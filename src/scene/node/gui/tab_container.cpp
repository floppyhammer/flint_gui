#include "tab_container.h"

namespace Flint {

void TabContainer::adjust_layout() {
    Container::adjust_layout();

    for (int i = 0; i < children.size(); i++) {
        children[i]->set_visibility(false);
        if (i == current_tab) {
            children[i]->set_visibility(true);
        }
    }
}

void TabContainer::update(double dt) {
    Control::update(dt);
}

Vec2F TabContainer::calculate_minimum_size() const {
    Vec2F min_size;

    uint32_t visible_child_count = 0;

    // Add every child's minimum size.
    for (auto &child : children) {
        if (child->is_gui_node()) {
            auto cast_child = dynamic_cast<Control *>(child.get());
            auto child_min_size = cast_child->calculate_minimum_size();
        }
    }

    return min_size.max(minimum_size);
}

} // namespace Flint
