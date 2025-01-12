#include "check_button.h"

#include "../../resources/default_resource.h"

namespace Flint {

CheckButton::CheckButton() {
    type = NodeType::CheckButton;

    toggle_mode = true;

    label->set_text("Check Button");

    theme_pressed = theme_normal;

    icon_normal_ = ResourceManager::get_singleton()->load<VectorImage>("assets/icons/CheckBox_Unchecked.svg");
    icon_pressed_ = ResourceManager::get_singleton()->load<VectorImage>("assets/icons/CheckBox_Checked.svg");
}

} // namespace Flint
