#include "mesh_instance_3d.h"

namespace Flint {
    void MeshInstance3D::set_mesh(const Mesh &p_mesh) {
        mesh = p_mesh;
    }

    Mesh MeshInstance3D::get_mesh() const {
        return mesh;
    }

    void self_draw() {

    }
}
