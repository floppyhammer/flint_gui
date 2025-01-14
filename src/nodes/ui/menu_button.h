#ifndef FLINT_MENU_BUTTON_H
#define FLINT_MENU_BUTTON_H

#include "button.h"

namespace Flint {

class PopupMenu;
class ScrollContainer;

class MenuButton : public Button {
public:
    MenuButton();

    std::weak_ptr<PopupMenu> get_popup_menu() const;

    void connect_signal(const std::string &signal, const AnyCallable<void> &callback) override;

    /// Manually select an item.
    void select_item(uint32_t item_index);

    std::optional<uint32_t> get_selected_item_index() const;

    std::string get_selected_item_text() const;

protected:
    std::optional<uint32_t> selected_item_index;

    std::shared_ptr<PopupMenu> menu;

    std::vector<AnyCallable<void>> selected_callbacks;

    void when_item_selected(uint32_t item_index);
};

} // namespace Flint

#endif // FLINT_MENU_BUTTON_H
