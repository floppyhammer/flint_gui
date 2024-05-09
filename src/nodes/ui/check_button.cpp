#include "check_button.h"

#include "../../resources/default_resource.h"

namespace Flint {

CheckButton::CheckButton() {
    type = NodeType::CheckButton;

    toggle_mode = true;

    label->set_text("Check Button");

    theme_pressed = theme_normal;

    icon_rect->set_custom_minimum_size({24, 24});

    icon_checked = ResourceManager::get_singleton()->load<VectorImage>("../assets/icons/CheckBox_Checked.svg");
    icon_unchecked = ResourceManager::get_singleton()->load<VectorImage>("../assets/icons/CheckBox_Unchecked.svg");

    icon_rect->set_texture(icon_unchecked);

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
