#ifndef FLINT_NODE2D_H
#define FLINT_NODE2D_H

#include "../../common/geometry.h"
#include "../../render/mvp.h"
#include "../../resources/surface.h"
#include "../../servers/vector_server.h"
#include "../node.h"

namespace Flint {

class Node2d : public Node {
public:
    Node2d();

    // Transform.
    // ----------------------------
    Vec2F position{0};
    Vec2F scale{1};
    float rotation = 0;
    Vec2F offset{0}; // Center as the origin.
    // ----------------------------

    Vec2F get_global_position() const;

protected:
    void draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    std::shared_ptr<Surface2d> surface;

    VectorPath position_indicator;
};

} // namespace Flint

#endif // FLINT_NODE2D_H
