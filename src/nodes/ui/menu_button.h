#ifndef FLINT_MENU_BUTTON_H
#define FLINT_MENU_BUTTON_H

#include "button.h"

namespace Flint {

class PopupMenu;

class MenuButton : public Button {
public:
    MenuButton();

protected:
    std::shared_ptr<PopupMenu> menu;
};

} // namespace Flint

#endif // FLINT_MENU_BUTTON_H
