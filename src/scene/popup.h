#ifndef FLINT_POPUP_H
#define FLINT_POPUP_H

#include <functional>

#include "window_proxy.h"

namespace Flint::Scene {

class Popup : public WindowProxy {
public:
    Popup(Vec2I size);
};

} // namespace Flint

#endif // FLINT_POPUP_H
