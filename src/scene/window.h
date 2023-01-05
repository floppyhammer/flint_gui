#ifndef FLINT_WINDOW_NODE_H
#define FLINT_WINDOW_NODE_H

#include "node.h"

using Pathfinder::ColorF;
using Pathfinder::Vec2I;

namespace Flint {

/// You can have multiple worlds which contain multiple cameras at the same time.
/// However, a Camera2d can only be effective in a 2d world. And the same applies to a Camera3d.
/// UI nodes have nothing to do with World.
class WindowNode : public Node {
    friend class SceneTree;

public:
    WindowNode(Vec2I _size);

    Vec2I get_size() const;

private:
    /// Window size.
    Vec2I size;
};

} // namespace Flint

#endif // FLINT_WINDOW_NODE_H
