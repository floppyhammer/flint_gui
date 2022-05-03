#ifndef FLINT_MODEL_H
#define FLINT_MODEL_H

#include "node_3d.h"
#include "../../../resources/mesh.h"
#include "../../../resources/texture.h"

namespace Flint {
    class Model : public Node3D {
    public:
        Model();

        void set_mesh(std::shared_ptr<Mesh3d> p_mesh);

    protected:
        std::shared_ptr<Mesh3d> mesh;

        void draw(VkCommandBuffer p_command_buffer) override;
    };
}

#endif //FLINT_MODEL_H
