#include "menu_button.h"

#include "../../resources/default_resource.h"
#include "popup_menu.h"

namespace Flint {

MenuButton::MenuButton() {
    type = NodeType::MenuButton;

    toggle_mode = true;

    label->set_text("Check Button");

    theme_pressed = theme_normal;

    menu = std::make_shared<PopupMenu>();
    add_embedded_child(menu);
    menu->set_visibility(false);

    pressed_callbacks.emplace_back([this] {
        menu->set_visibility(true);
        menu->set_position({0, position.y});
    });
}

} // namespace Flint
