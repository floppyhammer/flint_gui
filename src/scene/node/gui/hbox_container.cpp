#include "hbox_container.h"

namespace Flint {
    void HBoxContainer::adjust_layout() {
        std::vector<Control *> children_extended_from_control;
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto as_control = static_cast<Control *>(child.get());
                as_control->calculate_minimum_size();
                children_extended_from_control.push_back(as_control);
            }
        }

        for (auto &child: children_extended_from_control) {

        }
    }
}
