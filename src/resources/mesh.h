#ifndef FLINT_MESH_H
#define FLINT_MESH_H

#include "material.h"
#include "resource.h"
#include "surface.h"

/// A mesh consists of one or multiple surfaces and is managed by the resource manager.

namespace Flint {

class Mesh2d : public Resource {
public:
    Mesh2d() = default;

    explicit Mesh2d(const std::string &path);

    std::shared_ptr<Surface2d> surface;
};

class Mesh3d : public Resource {
public:
    explicit Mesh3d(const std::string &path);

    std::vector<std::shared_ptr<Surface3d>> surfaces;
};

} // namespace Flint

#endif // FLINT_MESH_H
