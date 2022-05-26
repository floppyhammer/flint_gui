#include "box_container.h"

namespace Flint {
    void BoxContainer::adjust_layout() {
        if (children.empty()) return;

        auto path = get_node_path();

        Vec2<float> min_size;
        std::vector<float> child_min_width;
        std::vector<Control *> expanding_children;
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                if (!cast_child->get_visibility()) continue;
                auto child_min_size = cast_child->calculate_minimum_size();

                if (horizontal) {
                    cast_child->set_position({min_size.x, 0});
                    cast_child->set_size({child_min_size.x, size.y});
                    child_min_width.push_back(child_min_size.x);
                    min_size.x += child_min_size.x;
                    min_size.y = std::max(size.y, child_min_size.y);
                } else {
                    cast_child->set_position({0, min_size.y});
                    cast_child->set_size({size.x, child_min_size.y});
                    child_min_width.push_back(child_min_size.y);
                    min_size.y += child_min_size.y;
                    min_size.x = std::max(size.x, child_min_size.x);
                }

                if (cast_child->sizing_flag == ContainerSizingFlag::EXPAND) {
                    expanding_children.push_back(cast_child);
                }
            }

            if (horizontal) {
                min_size.x += separation;
            } else {
                min_size.y += separation;
            }
        }

        float available_space_for_expanding = horizontal ? size.x - (min_size.x - separation) : size.y - (min_size.y - separation);
        uint32_t expanding_child_count = expanding_children.size();
        uint32_t valid_expanding_child_count = 0;

        if (available_space_for_expanding <= 0) {
            if (horizontal) {
                size = {min_size.x, size.y};
            } else {
                size = {size.x, min_size.y};
            }
        } else if (expanding_child_count != 0) {
            float extra_space_for_each_expanding_child = available_space_for_expanding / (float) expanding_child_count;

            float shift = 0;
            for (auto &child: children) {
                if (child->extended_from_which_base_node() == NodeType::Control) {
                    auto cast_child = dynamic_cast<Control *>(child.get());
                    if (!cast_child->get_visibility()) continue;
                    cast_child->set_position({shift, 0});
                    auto child_min_size = cast_child->calculate_minimum_size();

                    float min_wh = horizontal ? child_min_size.x : child_min_size.y;

                    if (std::find(expanding_children.begin(), expanding_children.end(), cast_child) !=
                        expanding_children.end()) {
                        if (min_wh > extra_space_for_each_expanding_child) {

                        } else {
                            min_wh += extra_space_for_each_expanding_child;
                        }
                    }

                    if (horizontal) {
                        cast_child->set_size({min_wh, size.y});
                    } else {
                        cast_child->set_size({size.x, min_wh});
                    }

                    shift += min_wh;
                }

                shift += separation;
            }
        }
    }

    void BoxContainer::update(double dt) {
        Control::update(dt);
    }

    Vec2<float> BoxContainer::calculate_minimum_size() const {
        Vec2<float> min_size;

        uint32_t visible_child_count = 0;

        // Add every child's minimum size.
        for (auto &child: children) {
            if (child->extended_from_which_base_node() == NodeType::Control) {
                auto cast_child = dynamic_cast<Control *>(child.get());
                auto child_min_size = cast_child->calculate_minimum_size();
                if (horizontal) {
                    min_size.x += child_min_size.x;
                    min_size.y = std::max(min_size.y, child_min_size.y);
                } else {
                    min_size.x = std::max(min_size.x, child_min_size.x);
                    min_size.y += child_min_size.y;
                }

                if (cast_child->get_visibility()) {
                    visible_child_count++;
                }
            }
        }

        // Take separations into account.
        if (visible_child_count > 0) {
            float total_separation_size = separation * (float) (visible_child_count - 1);
            if (horizontal) {
                min_size.x += total_separation_size;
            } else {
                min_size.y += total_separation_size;
            }
        }

        return min_size.max(minimum_size);
    }

    void BoxContainer::set_separation(float value) {
        separation = value;
        adjust_layout();
    }

    void BoxContainer::make_vertical() {
        horizontal = false;
    }

    void BoxContainer::make_horizontal() {
        horizontal = true;
    }
}
