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

    auto callback = [this](uint32_t item_index) { when_item_selected(item_index); };
    menu->connect_signal("item_selected", callback);
}

std::weak_ptr<PopupMenu> MenuButton::get_popup_menu() const {
    return menu;
}

void MenuButton::connect_signal(const std::string& signal, const AnyCallable<void>& callback) {
    NodeUi::connect_signal(signal, callback);

    if (signal == "item_selected") {
        selected_callbacks.push_back(callback);
    }
}

void MenuButton::select_item(uint32_t item_index) {
    when_item_selected(item_index);
}

std::string MenuButton::get_selected_item_text() const {
    if (selected_item_index > -1) {
        return menu->get_item_text(selected_item_index);
    }
    return "";
}

uint32_t MenuButton::get_selected_item_index() const {
    return selected_item_index;
}

void MenuButton::when_item_selected(uint32_t item_index) {
    set_text(menu->get_item_text(item_index));
    selected_item_index = item_index;

    for (auto& callback : selected_callbacks) {
        try {
            callback.operator()<uint32_t>(std::move(item_index));
        } catch (std::bad_any_cast&) {
            Logger::error("Mismatched signal argument types!");
        }
    }
}

} // namespace Flint
