#include "window.h"

namespace Flint {

Window::Window(Vec2I _size) {
    type = NodeType::Window;

    size = _size;
}

Vec2I Window::get_size() const {
    return size;
}

} // namespace Flint
