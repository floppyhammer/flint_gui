#include "popup.h"

namespace Flint {

Popup::Popup(Vec2I size) : WindowNode(size, false) {
    type = NodeType::Popup;
}

} // namespace Flint
