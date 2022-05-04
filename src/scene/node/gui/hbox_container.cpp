#include "hbox_container.h"

namespace Flint {
    void HBoxContainer::adjust_layout() {
        float shift_x = 0;
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                auto child_size = cast_child->calculate_minimum_size();
                cast_child->set_position({shift_x, 0});
                auto child_min_width = cast_child->calculate_minimum_size().x;
                cast_child->set_size({child_min_width, size.y});
                shift_x += child_min_width;
            }
        }
    }

    void HBoxContainer::update(double dt) {
        adjust_layout();

        Control::update(dt);
    }
}
