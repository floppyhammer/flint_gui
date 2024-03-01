#include "check_button.h"

#include "resources/default_resource.h"

namespace Flint {

CheckButton::CheckButton() {
    type = NodeType::CheckButton;

    toggle_mode = true;

    // Don't add the label as a child since it's not a normal node but part of the button.
    label = std::make_shared<Label>();
    label->set_text("Check Button");
    label->set_mouse_filter(MouseFilter::Ignore);
    label->set_horizontal_alignment(Alignment::Center);
    label->set_vertical_alignment(Alignment::Center);
    label->theme_background = StyleBox::from_empty();

    icon_rect->set_minimum_size({24, 24});

    icon_checked = ResourceManager::get_singleton()->load<VectorImage>("../assets/icons/CheckBox_Checked.svg");
    icon_unchecked = ResourceManager::get_singleton()->load<VectorImage>("../assets/icons/CheckBox_Unchecked.svg");

    icon_rect->set_texture(icon_checked);

    // set_icon_expand(true);

    toggled_callbacks.emplace_back([this](bool p_pressed) {
        if (p_pressed) {
            icon_rect->set_texture(icon_checked);
        } else {
            icon_rect->set_texture(icon_unchecked);
        }
    });
}

void CheckButton::set_checked_icon(const std::shared_ptr<Image> &icon) {
    icon_checked = icon;
}

void CheckButton::set_unchecked_icon(const std::shared_ptr<Image> &icon) {
    icon_unchecked = icon;
}

} // namespace Flint
