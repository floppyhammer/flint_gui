#include "margin_container.h"

namespace Flint {
    void MarginContainer::adjust_layout() {
        if (parent->extended_from_which_base_node() != NodeType::Control)
            return;

        auto cast_parent = dynamic_cast<Control *>(parent);
        set_size(cast_parent->get_size());

        Vec2<float> new_position;
        new_position.x += margin_left;
        new_position.y += margin_top;

        auto new_size = size;
        new_size.x -= margin_left + margin_right;
        new_size.y -= margin_top + margin_bottom;

        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                cast_child->set_position(new_position);
                cast_child->set_size(new_size);
            }
        }
    }

    void MarginContainer::update(double dt) {
        adjust_layout();

        Control::update(dt);
    }
}
