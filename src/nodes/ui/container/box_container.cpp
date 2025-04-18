#include "box_container.h"

namespace revector {

void BoxContainer::adjust_layout() {
    if (children.empty()) {
        return;
    }

    auto path = get_node_path();

    std::vector<NodeUi *> expanding_children;

    std::vector<NodeUi *> ui_children = get_visible_ui_children();

    // In the first loop, we need to do some calculation.
    for (auto &ui_child : ui_children) {
        if (horizontal) {
            if (ui_child->container_sizing.expand_h) {
                expanding_children.push_back(ui_child);
            }
        } else {
            if (ui_child->container_sizing.expand_v) {
                expanding_children.push_back(ui_child);
            }
        }
    }

    auto effective_min_size = get_effective_minimum_size();

    // Space available for child expanding.
    float available_space_for_expanding;
    if (horizontal) {
        available_space_for_expanding = size.x - effective_min_size.x;
    } else {
        available_space_for_expanding = size.y - effective_min_size.y;
    }

    // If the container is not large enough, readjust it to contain all its children.
    size = size.max(effective_min_size);

    uint32_t expanding_child_count = expanding_children.size();

    // FIXME: same expanding space is not optimal.
    float extra_space_for_each_expanding_child = available_space_for_expanding / (float)expanding_child_count;

    float pos_shift = 0;

    // In the second loop, we set child sizes and positions.
    for (auto &ui_child : ui_children) {
        auto child_min_size = ui_child->get_effective_minimum_size();

        float real_space = horizontal ? child_min_size.x : child_min_size.y;
        float occupied_space = real_space;

        if (extra_space_for_each_expanding_child > 0) {
            if (std::find(expanding_children.begin(), expanding_children.end(), ui_child) != expanding_children.end()) {
                occupied_space += extra_space_for_each_expanding_child;
            }
        }

        if (horizontal) {
            float pos_x = 0;
            float pos_y = 0;
            float height = 0;

            // Handle horizontal sizing.
            switch (ui_child->container_sizing.flag_h) {
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
            switch (ui_child->container_sizing.flag_v) {
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

            ui_child->set_position({pos_x, pos_y});
            ui_child->set_size({real_space, height});
        } else {
            float pos_x = 0;
            float pos_y = 0;
            float width = 0;

            // Handle vertical sizing.
            switch (ui_child->container_sizing.flag_v) {
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
            switch (ui_child->container_sizing.flag_h) {
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

            ui_child->set_position({pos_x, pos_y});
            ui_child->set_size({width, real_space});
        }

        pos_shift += occupied_space + separation;
    }
}

void BoxContainer::calc_minimum_size() {
    Vec2F min_size = {0, 0};

    std::vector<NodeUi *> ui_children = get_visible_ui_children();

    // Add every child's minimum size.
    for (auto &ui_child : ui_children) {
        auto child_min_size = ui_child->get_effective_minimum_size();

        if (horizontal) {
            min_size.x += child_min_size.x;
            min_size.y = std::max(min_size.y, child_min_size.y);
        } else {
            min_size.x = std::max(min_size.x, child_min_size.x);
            min_size.y += child_min_size.y;
        }
    }

    // Take separation into account.
    if (!ui_children.empty()) {
        float total_separation_size = separation * (ui_children.size() - 1);
        if (horizontal) {
            min_size.x += total_separation_size;
        } else {
            min_size.y += total_separation_size;
        }
    }

    calculated_minimum_size = min_size;
}

void BoxContainer::set_separation(float new_separation) {
    if (separation == new_separation) {
        return;
    }

    separation = new_separation;
}

} // namespace revector
