#ifndef FLINT_CHECK_BUTTON_H
#define FLINT_CHECK_BUTTON_H

#include "button.h"

namespace Flint {

class CheckButton : public Button {
public:
    CheckButton();

    void set_checked_icon(const std::shared_ptr<Image> &icon);

    void set_unchecked_icon(const std::shared_ptr<Image> &icon);

protected:
    std::shared_ptr<Image> icon_unchecked;
    std::shared_ptr<Image> icon_checked;
};

} // namespace Flint

#endif // FLINT_CHECK_BUTTON_H
