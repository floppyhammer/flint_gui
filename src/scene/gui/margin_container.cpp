#include "margin_container.h"

namespace Flint {
void MarginContainer::adjust_layout() {
    // Child's local position.
    Vec2F child_position;
    child_position.x += margin.left;
    child_position.y += margin.top;

    auto child_size = size;
    child_size.x -= margin.left + margin.right;
    child_size.y -= margin.top + margin.bottom;

    for (auto &child : children) {
        if (child->is_gui_node()) {
            auto cast_child = dynamic_cast<Control *>(child.get());
            cast_child->set_position(child_position);

            cast_child->set_size(child_size);
        }
    }
}

Vec2F MarginContainer::calc_minimum_size() const {
    Vec2F margin_size = {margin.left + margin.right, margin.top + margin.bottom};

    Vec2F max_child_min_size;
    for (auto &child : children) {
        if (child->is_gui_node()) {
            auto cast_child = dynamic_cast<Control *>(child.get());
            auto child_min_size = cast_child->calc_minimum_size() + margin_size;

            max_child_min_size = max_child_min_size.max(child_min_size);
        }
    }

    return minimum_size.max(max_child_min_size);
}

void MarginContainer::update(double dt) {
    adjust_layout();

    Control::update(dt);
}

void MarginContainer::set_margin_all(float value) {
    set_margin({value, value, value, value});
}

void MarginContainer::set_margin(const RectF &p_margin) {
    margin = p_margin;
}
} // namespace Flint
