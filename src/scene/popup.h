#ifndef FLINT_POPUP_H
#define FLINT_POPUP_H

#include <functional>

#include "window.h"

namespace Flint {

class Popup : public WindowNode {
public:
    Popup(Vec2I size);
};

} // namespace Flint

#endif // FLINT_POPUP_H
