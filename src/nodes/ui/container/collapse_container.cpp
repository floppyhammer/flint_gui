#include "collapse_container.h"

#include <string>

#include "../../../common/utils.h"
#include "../../../resources/default_resource.h"

namespace Flint {

void cc(bool p) {
    int a = 1;
}

CollapseContainer::CollapseContainer() {
    type = NodeType::CollapseContainer;

    auto default_theme = DefaultResource::get_singleton()->get_default_theme();

    theme_title_bar_ = std::make_optional(default_theme->collapsing_panel.styles["title_bar"]);
    theme_title_bar_->corner_radii = {8, 8, 0, 0};
    theme_panel_ = std::make_optional(default_theme->collapsing_panel.styles["background"]);

    collapse_button_ = std::make_shared<Button>();
    collapse_button_->set_custom_minimum_size({0, title_bar_height_});
    // collapse_button_->set_icon_normal(std::make_shared<VectorImage>("assets/icons/ArrowDown.svg"));
    // collapse_button_->set_icon_pressed(std::make_shared<VectorImage>("assets/icons/ArrowRight.svg"));
    collapse_button_->set_text("Collapsing Container");
    collapse_button_->set_flat(true);
    collapse_button_->set_toggle_mode(true);
    collapse_button_->connect_signal("toggled", [this](bool p_pressed = true) {
        set_collapse(!p_pressed);
    });

    add_embedded_child(collapse_button_);

    container_sizing.expand_h = true;
}

void CollapseContainer::adjust_layout() {
    // Get the minimum size.
    auto min_size = get_effective_minimum_size();

    // Adjust own size.
    size = size.max(min_size);

    if (!is_inside_container() && collapsed_) {
        size = min_size;
    }

    collapse_button_->set_size({size.x, title_bar_height_});

    // Adjust child size.
    for (auto &child : children) {
        if (child->is_ui_node()) {
            auto child_size = size;
            child_size -= Vec2F{margin_ * 2, margin_ * 2 + title_bar_height_};
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            cast_child->set_position({margin_, title_bar_height_ + margin_});
            cast_child->set_size(child_size);
        }

        child->set_visibility(!collapsed_);
    }
}
void CollapseContainer::set_collapse(bool collapse) {
    if (collapsed_ == collapse) {
        return;
    }
    collapsed_ = collapse;

    if (!collapse) {
        this->size_before_collapse_ = this->size;
    }

    if (collapse) {
        this->size = this->size_before_collapse_;
    }

    if (collapse) {
        theme_title_bar_->corner_radii = {8, 8, 8, 8};
    } else {
        theme_title_bar_->corner_radii = {8, 8, 0, 0};
    }
}

void CollapseContainer::calc_minimum_size() {
    // Get the minimum child size.
    Vec2F min_child_size{};
    if (!collapsed_) {
        for (const auto &child : children) {
            if (child->is_ui_node()) {
                auto cast_child = dynamic_cast<NodeUi *>(child.get());
                auto child_min_size = cast_child->get_effective_minimum_size();
                min_child_size = min_child_size.max(child_min_size);
            }
        }

        min_child_size += Vec2F{margin_, margin_} * 2;
    }

    Vec2F min_embeded_child_size{};
    for (const auto &child : embedded_children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            auto child_min_size = cast_child->get_effective_minimum_size();
            min_embeded_child_size = min_embeded_child_size.max(child_min_size);
        }
    }

    calculated_minimum_size = min_child_size + min_embeded_child_size;
}

void CollapseContainer::set_title(std::string title) {
    collapse_button_->set_text(title);
}

void CollapseContainer::update(double dt) {
    NodeUi::update(dt);

    adjust_layout();
}

void CollapseContainer::draw() {
    if (!visible_) {
        return;
    }

    auto vector_server = VectorServer::get_singleton();

    auto global_position = get_global_position();

    if (theme_panel_.has_value()) {
        vector_server->draw_style_box(theme_panel_.value(), global_position, size);
    }

    if (theme_title_bar_.has_value()) {
        auto title_bar_size = size;
        title_bar_size.y = title_bar_height_;
        vector_server->draw_style_box(theme_title_bar_.value(), global_position, title_bar_size);
    }
}

} // namespace Flint
