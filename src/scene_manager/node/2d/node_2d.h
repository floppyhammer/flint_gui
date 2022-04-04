#ifndef FLINT_NODE_2D_H
#define FLINT_NODE_2D_H

#include "../node.h"
#include "../../../common/vec2.h"

namespace Flint {
    class Node2d : public Node {
    public:
        Node2d();

        // Transform.
        // ------------------------------------------
        Vec2<float> position{0};

        Vec2<float> scale{1};

        float rotation = 0;
        // ------------------------------------------
    };
}

#endif //FLINT_NODE_2D_H
