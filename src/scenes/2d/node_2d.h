#ifndef FLINT_NODE_2D_H
#define FLINT_NODE_2D_H

#include "../node.h"
#include "../../common/vec2.h"

namespace Flint {
    class Node2D : public Node {
    public:
        // Transform.
        // ------------------------------------------
        Vec2<float> position = Vec2<float>(0);

        float rotation = 0;

        Vec2<float> scale = Vec2<float>(1);
        // ------------------------------------------
    };
}

#endif //FLINT_NODE_2D_H
