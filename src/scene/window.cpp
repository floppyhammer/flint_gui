#include "window.h"

namespace Flint {

WindowNode::WindowNode(Vec2I _size) {
    type = NodeType::Window;

    size = _size;
}

Vec2I WindowNode::get_size() const {
    return size;
}

} // namespace Flint
