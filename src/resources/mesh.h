#ifndef FLINT_MESH_H
#define FLINT_MESH_H

#include "resource.h"
#include "surface.h"
#include "material.h"

class Mesh2d : public Resource {
public:
    Mesh2d();
    explicit Mesh2d(const std::string &path);

    std::shared_ptr<Surface2d> surface;
};

class Mesh3d : public Resource {
public:
    explicit Mesh3d(const std::string &path);

    std::vector<std::shared_ptr<Surface3d>> surfaces;
};

#endif //FLINT_MESH_H
