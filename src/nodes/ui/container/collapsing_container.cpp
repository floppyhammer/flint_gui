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

    collapse_button_ = std::make_shared<Button>();
    collapse_button_->set_icon(std::make_shared<VectorImage>("../assets/icons/ArrowDown.svg"));
    collapse_button_->set_text("");
    collapse_button_->set_flat(true);
    title_bar_container_->add_child(collapse_button_);

    title_label_ = std::make_shared<Label>();
    title_label_->set_text("Collasping Panel");
    title_label_->set_mouse_filter(MouseFilter::Ignore);
    title_label_->set_horizontal_alignment(Alignment::Center);
    title_label_->set_vertical_alignment(Alignment::Center);
    title_label_->set_text_style(TextStyle{default_theme->button.colors["text"]});
    title_label_->theme_background = StyleBox::from_empty();
    title_bar_container_->add_child(title_label_);

    add_embedded_child(title_bar_container_);
}

void CollapsingContainer::set_theme_panel(StyleBox style_box) {
    theme_panel_ = std::make_optional(style_box);
}

void CollapsingContainer::update(double dt) {
    NodeUi::update(dt);
    title_bar_container_->set_size({size.x, title_bar_height_});
}

void CollapsingContainer::draw() {
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
