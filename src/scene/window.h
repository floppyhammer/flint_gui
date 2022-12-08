#ifndef FLINT_WINDOW_H
#define FLINT_WINDOW_H

#include "node.h"

using Pathfinder::ColorF;
using Pathfinder::Vec2I;

namespace Flint {

/// You can have multiple worlds which contain multiple cameras at the same time.
/// However, a Camera2d can only be effective in a 2d world. And the same applies to a Camera3d.
/// UI nodes have nothing to do with World.
class Window : public Node {
    friend class SceneTree;

public:
    Window();

    Vec2I get_size() const;

    /// Window size.
    Vec2I size;
};

} // namespace Flint

#endif // FLINT_WINDOW_H
