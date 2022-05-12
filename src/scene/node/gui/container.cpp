#include "container.h"

namespace Flint {
    void Container::adjust_layout() {
        auto max_size = size.max(minimum_size);
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                auto child_size = cast_child->calculate_minimum_size();
                cast_child->set_position({0, 0});
                auto child_min_size = cast_child->calculate_minimum_size();
                max_size = max_size.max(child_min_size);
            }
        }

        set_size(max_size);

        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                cast_child->set_size(max_size);
            }
        }
    }
}
