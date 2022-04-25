#ifndef FLINT_NODE_2D_H
#define FLINT_NODE_2D_H

#include "../node.h"
#include "../../../common/vec2.h"
#include "../../../render/mvp_buffer.h"
#include "../../../resources/mesh.h"

namespace Flint {
    class Node2d : public Node {
    public:
        Node2d();

        // Transform.
        // ----------------------------
        Vec2<float> position{0};
        Vec2<float> scale{1};
        Vec2<float> offset{0}; // Center as the origin.
        float rotation = 0;
        // ----------------------------

        [[nodiscard]] Vec2<float> get_global_position() const;

    protected:
        std::shared_ptr<Mesh2d> mesh;
    };
}

#endif //FLINT_NODE_2D_H
