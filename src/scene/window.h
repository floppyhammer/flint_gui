#ifndef FLINT_WINDOW_NODE_H
#define FLINT_WINDOW_NODE_H

#include "node.h"

using Pathfinder::ColorF;
using Pathfinder::Vec2I;

namespace Flint {

class WindowNode : public Node {
    friend class SceneTree;

public:
    WindowNode(Vec2I _size, bool _dummy);

    Vec2I get_size() const;

private:
    /// Window size.
    Vec2I size;

    // Dummy window for headless use.
    bool dummy = false;
};

} // namespace Flint

#endif // FLINT_WINDOW_NODE_H
