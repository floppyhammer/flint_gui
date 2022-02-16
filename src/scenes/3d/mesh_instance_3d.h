//
// Created by chy on 2/16/2022.
//

#ifndef VULKAN_DEMO_APP_MESH_INSTANCE_3D_H
#define VULKAN_DEMO_APP_MESH_INSTANCE_3D_H

#include "../../rendering/mesh.h"

class MeshInstance3D {
public:
    void set_mesh(const Mesh &p_mesh);
    Mesh get_mesh() const;

private:
    Mesh mesh;
};

#endif //VULKAN_DEMO_APP_MESH_INSTANCE_3D_H
