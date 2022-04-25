#ifndef FLINT_DEFAULT_RESOURCE_H
#define FLINT_DEFAULT_RESOURCE_H

#include "mesh.h"

class DefaultResource {
public:
    DefaultResource() {
        default_mesh_2d = Mesh2d::from_default();
    }

    static DefaultResource &get_singleton() {
        static DefaultResource singleton;
        return singleton;
    }

    // This default mesh should not be modified.
    std::shared_ptr<Mesh2d> default_mesh_2d;
};

#endif //FLINT_DEFAULT_RESOURCE_H
