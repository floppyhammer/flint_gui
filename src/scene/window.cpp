#include "window.h"

namespace Flint {

WindowNode::WindowNode(Vec2I _size, bool _dummy) {
    type = NodeType::Window;

    size = _size;

    dummy = _dummy;
}

Vec2I WindowNode::get_size() const {
    return size;
}

} // namespace Flint
