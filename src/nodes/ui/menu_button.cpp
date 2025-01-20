#include "menu_button.h"

#include "../../resources/default_resource.h"
#include "../../servers/debug_server.h"
#include "container/scroll_container.h"
#include "popup_menu.h"

namespace Flint {

MenuButton::MenuButton() {
    type = NodeType::MenuButton;

    label->set_text("Menu Button");

    theme_pressed = theme_normal;

    menu = std::make_shared<PopupMenu>();
    menu->render_layer = 1;
    add_embedded_child(menu);

    menu->set_visibility(false);

    pressed_callbacks.emplace_back([this] {
        if (menu->get_item_count() ==0 ) {
            return;
        }
        menu->set_position(Vec2F{0, size.y});
        auto global_pos = get_global_position();
        menu->calc_global_position(global_pos);
        menu->set_visibility(true);
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
    if (selected_item_index.has_value()) {
        return menu->get_item_text(selected_item_index.value());
    }
    return "";
}

std::optional<uint32_t> MenuButton::get_selected_item_index() const {
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
