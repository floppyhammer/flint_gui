#ifndef FLINT_DEFAULT_RESOURCE_H
#define FLINT_DEFAULT_RESOURCE_H

#include "surface.h"
#include "mesh.h"

class DefaultResource {
public:
    DefaultResource() {
        default_surface_2d = Surface2d::from_default();
        default_mesh_2d = std::make_shared<Mesh2d>();
        default_mesh_2d->surface = default_surface_2d;

        default_material_2d = std::make_shared<Material2d>();
        default_material_2d->set_texture(Texture::from_empty(4, 4));
    }

    static DefaultResource &get_singleton() {
        static DefaultResource singleton;
        return singleton;
    }

    // Default resources should never be modified.
    std::shared_ptr<Surface2d> default_surface_2d;

    std::shared_ptr<Mesh2d> default_mesh_2d;

    std::shared_ptr<Material2d> default_material_2d;
};

#endif //FLINT_DEFAULT_RESOURCE_H
