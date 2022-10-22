#include "stack_container.h"

namespace Flint {
void StackContainer::adjust_layout() {
    if (children.empty()) {
        return;
    }

    auto path = get_node_path();

    Vec2F total_size;
    std::vector<float> max_child_width;
    std::vector<Control *> expanding_children;

    std::vector<std::pair<Control *, Vec2F>> child_cache;

    // In the first loop, we only do some statistics.
    for (auto &child : children) {
        // We only care about visible GUI nodes in a container.
        if (!child->get_visibility() || !child->is_gui_node()) {
            continue;
        }

        auto cast_child = dynamic_cast<Control *>(child.get());

        auto child_min_size = cast_child->calculate_minimum_size();

        child_cache.push_back(std::pair(cast_child, child_min_size));

        if (horizontal) {
            total_size.x += child_min_size.x + separation;

            if (cast_child->container_sizing.expand_h) {
                expanding_children.push_back(cast_child);
            }
        } else {
            total_size.y += child_min_size.y + separation;

            if (cast_child->container_sizing.expand_v) {
                expanding_children.push_back(cast_child);
            }
        }
    }

    float available_space_for_expanding;
    if (horizontal) {
        // Subtract redundant separation.
        total_size.x -= separation;

        available_space_for_expanding = size.x - total_size.x;
    } else {
        // Subtract redundant separation.
        total_size.y -= separation;

        available_space_for_expanding = size.y - total_size.y;
    }

    // If the container is not large enough, readjust it to contain all its children.
    size = size.max(total_size);

    uint32_t expanding_child_count = expanding_children.size();

    float extra_space_for_each_expanding_child = available_space_for_expanding / (float)expanding_child_count;

    float pos_shift = 0;

    // In the second loop, we set child sizes and positions.
    for (auto &pair : child_cache) {
        auto cast_child = pair.first;

        auto child_min_size = pair.second;

        float real_space = horizontal ? child_min_size.x : child_min_size.y;
        float occupied_space = real_space;

        if (extra_space_for_each_expanding_child > 0) {
            if (std::find(expanding_children.begin(), expanding_children.end(), cast_child) !=
                expanding_children.end()) {
                occupied_space += extra_space_for_each_expanding_child;
            }
        }

        if (horizontal) {
            float pos_x = 0;
            float pos_y = 0;
            float height = 0;

            // Handle horizontal sizing.
            switch (cast_child->container_sizing.flag_h) {
                case ContainerSizingFlag::Fill: {
                    real_space = occupied_space;
                    pos_x = pos_shift;
                } break;
                case ContainerSizingFlag::ShrinkStart: {
                    pos_x = pos_shift;
                } break;
                case ContainerSizingFlag::ShrinkCenter: {
                    pos_x = pos_shift + (occupied_space - real_space) * 0.5f;
                } break;
                case ContainerSizingFlag::ShrinkEnd: {
                    pos_x = pos_shift + (occupied_space - real_space);
                } break;
            }

            // Handle vertical sizing.
            switch (cast_child->container_sizing.flag_v) {
                case ContainerSizingFlag::Fill: {
                    height = size.y;
                    pos_y = 0;
                } break;
                case ContainerSizingFlag::ShrinkStart: {
                    height = child_min_size.y;
                    pos_y = 0;
                } break;
                case ContainerSizingFlag::ShrinkCenter: {
                    height = child_min_size.y;
                    pos_y = (size.y - height) * 0.5f;
                } break;
                case ContainerSizingFlag::ShrinkEnd: {
                    height = child_min_size.y;
                    pos_y = size.y - height;
                } break;
            }

            cast_child->set_position({pos_x, pos_y});
            cast_child->set_size({real_space, height});
        } else {
            float pos_x = 0;
            float pos_y = 0;
            float width = 0;

            // Handle vertical sizing.
            switch (cast_child->container_sizing.flag_v) {
                case ContainerSizingFlag::Fill: {
                    real_space = occupied_space;
                    pos_y = pos_shift;
                } break;
                case ContainerSizingFlag::ShrinkStart: {
                    pos_y = pos_shift;
                } break;
                case ContainerSizingFlag::ShrinkCenter: {
                    pos_y = pos_shift + (occupied_space - real_space) * 0.5f;
                } break;
                case ContainerSizingFlag::ShrinkEnd: {
                    pos_y = pos_shift + (occupied_space - real_space);
                } break;
            }

            // Handle horizontal sizing.
            switch (cast_child->container_sizing.flag_h) {
                case ContainerSizingFlag::Fill: {
                    width = size.x;
                    pos_x = 0;
                } break;
                case ContainerSizingFlag::ShrinkStart: {
                    width = child_min_size.x;
                    pos_x = 0;
                } break;
                case ContainerSizingFlag::ShrinkCenter: {
                    width = child_min_size.x;
                    pos_x = (size.x - width) * 0.5f;
                } break;
                case ContainerSizingFlag::ShrinkEnd: {
                    width = child_min_size.x;
                    pos_x = size.x - width;
                } break;
            }

            cast_child->set_position({pos_x, pos_y});
            cast_child->set_size({width, real_space});
        }

        pos_shift += occupied_space + separation;
    }
}

void StackContainer::update(double dt) {
    Control::update(dt);
}

Vec2F StackContainer::calculate_minimum_size() const {
    Vec2F min_size;

    uint32_t visible_child_count = 0;

    // Add every child's minimum size.
    for (auto &child : children) {
        if (child->is_gui_node()) {
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

    // Take separation into account.
    if (visible_child_count > 0) {
        float total_separation_size = separation * (float)(visible_child_count - 1);
        if (horizontal) {
            min_size.x += total_separation_size;
        } else {
            min_size.y += total_separation_size;
        }
    }

    return min_size.max(minimum_size);
}

void StackContainer::set_separation(float new_separation) {
    if (separation == new_separation) {
        return;
    }

    separation = new_separation;
    adjust_layout();
}
} // namespace Flint
