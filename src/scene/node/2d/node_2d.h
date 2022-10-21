#ifndef FLINT_NODE_2D_H
#define FLINT_NODE_2D_H

#include "../../../common/geometry.h"
#include "../../../render/mvp.h"
#include "../../../resources/mesh.h"
#include "../../../servers/vector_server.h"
#include "../node.h"

namespace Flint {
class Node2d : public Node {
public:
    Node2d();

    // Transform.
    // ----------------------------
    Vec2F position{0};
    Vec2F scale{1};
    Vec2F offset{0}; // Center as the origin.
    float rotation = 0;
    // ----------------------------

    Vec2F get_global_position() const;

protected:
    void draw(VkCommandBuffer p_command_buffer) override;

    std::shared_ptr<Mesh2d> mesh;

    VectorPath position_indicator;
};
} // namespace Flint

#endif // FLINT_NODE_2D_H
