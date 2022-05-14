#include "hbox_container.h"

namespace Flint {
    void HBoxContainer::adjust_layout() {
        if (children.empty()) return;

        Vec2<float> min_size;
        std::vector<float> child_min_width;
        std::vector<Control *> expanding_children;
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                auto child_min_size = cast_child->calculate_minimum_size();
                cast_child->set_position({min_size.x, 0});
                cast_child->set_size({child_min_size.x, size.y});
                child_min_width.push_back(child_min_size.x);

                min_size.x += child_min_size.x;
                min_size.y = std::max(size.y, child_min_size.y);

                if (cast_child->sizing_flag == ContainerSizingFlag::EXPAND) {
                    expanding_children.push_back(cast_child);
                }
            }

            min_size.x += separation;
        }

        float available_space_for_expanding = size.x - (min_size.x - separation);
        uint32_t expanding_child_count = expanding_children.size();
        uint32_t valid_expanding_child_count = 0;

        if (available_space_for_expanding < 0) {
            size = {min_size.x, size.y};
        } else {
            float extra_space_for_each_expanding_child = available_space_for_expanding / (float) expanding_child_count;

            float shift_x = 0;
            for (auto &child: children) {
                if (child->extended_from_which_base_node() == NodeType::Control) {
                    auto cast_child = dynamic_cast<Control *>(child.get());
                    cast_child->set_position({shift_x, 0});
                    auto child_min_size = cast_child->calculate_minimum_size();

                    if (std::find(expanding_children.begin(), expanding_children.end(), cast_child) != expanding_children.end()) {
                        if (child_min_size.x > extra_space_for_each_expanding_child) {

                        } else {
                            child_min_size.x += extra_space_for_each_expanding_child;
                        }
                    }

                    cast_child->set_size({child_min_size.x, size.y});

                    shift_x += child_min_size.x;
                }

                shift_x += separation;
            }
        }
    }

    void HBoxContainer::update(double dt) {
        Control::update(dt);
    }

    Vec2<float> HBoxContainer::calculate_minimum_size() const {
        Vec2<float> min_size;

        // Add every child's minimum size.
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                min_size += cast_child->calculate_minimum_size();
            }
        }

        // Take separations into account.
        min_size.x += separation * (float) (children.size() - 1);

        return min_size.max(minimum_size);
    }
}
