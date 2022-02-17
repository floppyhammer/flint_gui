#include "mesh_instance_3d.h"

#include <utility>

namespace Flint {
    void MeshInstance3D::set_mesh(std::shared_ptr<Mesh> p_mesh) {
        mesh = std::move(p_mesh);

        createIndexBuffer();
        createUniformBuffers();
        createVertexBuffer();
    }

    std::shared_ptr<Mesh> MeshInstance3D::get_mesh() const {
        return mesh;
    }

    void self_draw() {

    }
}
