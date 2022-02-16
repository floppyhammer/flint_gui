#ifndef FLINT_MESH_INSTANCE_3D_H
#define FLINT_MESH_INSTANCE_3D_H

#include "node_3d.h"
#include "../../rendering/mesh.h"

namespace Flint {
    class MeshInstance3D : public Node3D {
    public:
        void set_mesh(const Mesh &p_mesh);

        Mesh get_mesh() const;

        void self_draw() override;

    private:
        Mesh mesh;
    };
}

#endif //FLINT_MESH_INSTANCE_3D_H
