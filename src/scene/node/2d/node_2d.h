#ifndef FLINT_NODE_2D_H
#define FLINT_NODE_2D_H

#include "../node.h"
#include "../../../common/math/vec2.h"
#include "../../../render/mvp_buffer.h"
#include "../../../resources/mesh.h"
#include "../../../servers/vector_server.h"

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

        Vec2<float> get_global_position() const;

    protected:
        std::shared_ptr<Mesh2d> mesh;

        void draw(VkCommandBuffer p_command_buffer) override;

        VShape position_indicator;
    };
}

#endif //FLINT_NODE_2D_H
