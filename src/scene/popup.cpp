#include "popup.h"

namespace Flint::Scene {

Popup::Popup(Vec2I size) : WindowProxy(size, false) {
    type = NodeType::Popup;
}

} // namespace Flint
