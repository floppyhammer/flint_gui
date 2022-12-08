#include "window.h"

namespace Flint {

Window::Window() {
    type = NodeType::Window;
}

Vec2I Window::get_size() const {
    return size;
}

} // namespace Flint
