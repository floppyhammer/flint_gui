#include "vbox_container.h"

namespace Flint {
    void VBoxContainer::adjust_layout() {
        float shift_y = 0;
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                auto child_size = cast_child->calculate_minimum_size();
                cast_child->set_position({0, shift_y});
                auto child_min_height = cast_child->calculate_minimum_size().y;
                cast_child->set_size({size.x, child_min_height});
                shift_y += child_min_height;
            }

            shift_y += separation;
        }
    }

    void VBoxContainer::update(double dt) {
        adjust_layout();

        Control::update(dt);
    }

    Vec2<float> VBoxContainer::calculate_minimum_size() {
        Vec2<float> size;
        bool has_control_child = false;
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                auto child_size = cast_child->calculate_minimum_size();
                size += child_size;
                size.y += separation;
                has_control_child = true;
            }
        }
        if (has_control_child) size.y -= separation;
        return size.max(minimum_size);
    }
}
