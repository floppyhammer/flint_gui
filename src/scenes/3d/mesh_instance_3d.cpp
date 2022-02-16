//
// Created by chy on 2/16/2022.
//

#include "mesh_instance_3d.h"

void MeshInstance3D::set_mesh(const Mesh &p_mesh) {
    mesh = p_mesh;
}

Mesh MeshInstance3D::get_mesh() const {
    return mesh;
}
