#include "vbox_container.h"

namespace Flint {
    void VBoxContainer::adjust_layout() {
        float shift_y = 0;
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                auto child_min_size = cast_child->calculate_minimum_size();
                cast_child->set_position({0, shift_y});
                cast_child->set_size({size.x, child_min_size.y});
                shift_y += child_min_size.y;
            }

            shift_y += separation;
        }
    }

    void VBoxContainer::update(double dt) {
        Control::update(dt);
    }

    Vec2<float> VBoxContainer::calculate_minimum_size() const {
        Vec2<float> min_size;

        // Add every child's minimum size.
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                min_size += cast_child->calculate_minimum_size();
            }
        }

        // Take separations into account.
        min_size.y += separation * (float) (children.size() - 1);

        return min_size.max(minimum_size);
    }
}
