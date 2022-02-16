#ifndef FLINT_NODE_3D_H
#define FLINT_NODE_3D_H

#include "../node.h"
#include "../../common/vec3.h"
#include "../../rendering/rendering_server.h"

namespace Flint {
    class Node3D : public Node {
    public:
        // Transform.
        // ------------------------------------------
        Vec3<float> position = Vec3<float>(0);

        Vec3<float> rotation = Vec3<float>(0);

        Vec3<float> scale = Vec3<float>(1);
        // ------------------------------------------

    private:
        UniformBufferObject mvp;

        void update_uniform_buffer();
    };
}

#endif //FLINT_NODE_3D_H
