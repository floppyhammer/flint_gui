#include "collapsing_container.h"

#include <string>

#include "../../../common/utils.h"
#include "../../../resources/default_resource.h"

namespace Flint {

CollapsingContainer::CollapsingContainer() {
    type = NodeType::CollapsingContainer;

    auto default_theme = DefaultResource::get_singleton()->get_default_theme();

    theme_title_bar_ = std::make_optional(default_theme->collapsing_panel.styles["title_bar"]);
    theme_panel_ = std::make_optional(default_theme->collapsing_panel.styles["background"]);

    title_bar_container_ = std::make_shared<HBoxContainer>();
    title_bar_container_->set_custom_minimum_size({0, title_bar_height_});

    collapse_button_ = std::make_shared<Button>();
    collapse_button_->set_icon_normal(std::make_shared<VectorImage>("../assets/icons/ArrowDown.svg"));
    collapse_button_->set_icon_pressed(std::make_shared<VectorImage>("../assets/icons/ArrowRight.svg"));
    collapse_button_->set_text("");
    collapse_button_->set_flat(true);
    collapse_button_->set_toggle_mode(true);
    collapse_button_->set_icon_expand(true);
    title_bar_container_->add_child(collapse_button_);
    collapse_button_->connect_signal_toggled([this](bool p_pressed) {
        if (!this->collapsed_ && p_pressed) {
            this->size_before_collapse_ = this->size;
        }

        if (this->collapsed_ && !p_pressed) {
            this->size = this->size_before_collapse_;
        }

        this->collapsed_ = p_pressed;
    });

    title_label_ = std::make_shared<Label>();
    title_label_->set_text("Collasping Container");
    title_label_->set_mouse_filter(MouseFilter::Ignore);
    title_label_->set_horizontal_alignment(Alignment::Center);
    title_label_->set_vertical_alignment(Alignment::Center);
    title_label_->set_text_style(TextStyle{default_theme->button.colors["text"]});
    title_label_->theme_background = StyleBox::from_empty();
    title_bar_container_->add_child(title_label_);

    add_embedded_child(title_bar_container_);
}

void CollapsingContainer::adjust_layout() {
    // Get the minimum size.
    auto min_size = get_effective_minimum_size();

    // Adjust own size.
    size = size.max(min_size);

    if (collapsed_) {
        size = min_size;
    }

    title_bar_container_->set_size({size.x, title_bar_height_});

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

void CollapsingContainer::calc_minimum_size() {
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

void CollapsingContainer::update(double dt) {
    NodeUi::update(dt);

    adjust_layout();
}

void CollapsingContainer::draw() {
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
