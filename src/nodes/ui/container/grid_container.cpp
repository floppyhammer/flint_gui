#include "grid_container.h"

namespace revector {

void GridContainer::adjust_layout() {
    if (children.empty()) {
        return;
    }

    auto path = get_node_path();

    // In the first loop, we need do some calculation.
    for (int child_idx = 0; child_idx < children.size(); child_idx++) {
        auto &child = children[child_idx];

        // We only care about visible GUI nodes in a container.
        if (!child->get_visibility() || !child->is_ui_node()) {
            continue;
        }

        auto cast_child = dynamic_cast<NodeUi *>(child.get());

        int row_idx = child_idx / col_num;
        int col_idx = child_idx % col_num;

        float pos_x = col_idx * separation;
        for (int col = 0; col < col_idx; col++) {
            pos_x += max_col_widths[col];
        }
        float pos_y = row_idx * separation;
        for (int row = 0; row < row_idx; row++) {
            pos_y += max_row_heights[row];
        }

        cast_child->set_position({pos_x, pos_y});
        cast_child->set_size({max_col_widths[col_idx], max_row_heights[row_idx]});
    }
}

void GridContainer::calc_minimum_size() {
    int row_num = children.size() / col_num;

    float max_row_width = 0;
    max_row_heights.resize(row_num);

    for (int i = 0; i < row_num; i++) {
        float cur_row_width = 0;

        max_row_heights[i] = 0;

        for (int j = 0; j < col_num; j++) {
            int child_index = i * col_num + j;
            if (child_index >= children.size()) {
                break;
            }

            auto child = children[child_index];

            // We only care about visible GUI nodes in a container.
            if (!child->get_visibility() || !child->is_ui_node()) {
                continue;
            }

            auto cast_child = dynamic_cast<NodeUi *>(child.get());

            auto child_min_size = cast_child->get_effective_minimum_size();

            cur_row_width += child_min_size.x;
            max_row_heights[i] = std::max(max_row_heights[i], child_min_size.y);
        }

        cur_row_width += (col_num - 1) * separation;

        max_row_width = std::max(max_row_width, cur_row_width);
    }

    float max_col_height = 0;
    max_col_widths.resize(col_num);

    for (int j = 0; j < col_num; j++) {
        float cur_col_height = 0;

        for (int i = 0; i < row_num; i++) {
            int child_index = i * col_num + j;
            if (child_index >= children.size()) {
                break;
            }

            auto child = children[child_index];

            // We only care about visible GUI nodes in a container.
            if (!child->get_visibility() || !child->is_ui_node()) {
                continue;
            }

            auto cast_child = dynamic_cast<NodeUi *>(child.get());

            auto child_min_size = cast_child->get_effective_minimum_size();

            cur_col_height += child_min_size.y;
            max_col_widths[i] = std::max(max_col_widths[i], child_min_size.x);
        }

        cur_col_height += (col_num - 1) * separation;

        max_col_height = std::max(max_col_height, cur_col_height);
    }

    calculated_minimum_size = {max_row_width, max_col_height};
}

void GridContainer::set_separation(float new_separation) {
    if (separation == new_separation) {
        return;
    }

    separation = new_separation;
}

void GridContainer::set_column_number(uint32_t new_column_number) {
    col_num = new_column_number;
}

} // namespace revector
