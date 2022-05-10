#include "margin_container.h"

namespace Flint {
    void MarginContainer::adjust_layout() {
        if (parent->extended_from_which_base_node() != NodeType::Control)
            return;

        auto cast_parent = dynamic_cast<Control *>(parent);
        set_size(cast_parent->get_size());

        // Child's local position.
        Vec2<float> child_position;
        child_position.x += margin_left;
        child_position.y += margin_top;

        auto child_size = size;
        child_size.x -= margin_left + margin_right;
        child_size.y -= margin_top + margin_bottom;

        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                cast_child->set_position(child_position);
                cast_child->set_size(child_size);
            }
        }
    }

    void MarginContainer::update(double dt) {
        adjust_layout();

        Control::update(dt);
    }
}
