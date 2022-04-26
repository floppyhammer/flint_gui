#ifndef FLINT_DEFAULT_RESOURCE_H
#define FLINT_DEFAULT_RESOURCE_H

#include "surface.h"
#include "mesh.h"

class DefaultResource {
public:
    DefaultResource() {
        // Default vertices and indices data for 2D quad surface.
        const std::vector<Vertex> vertices = {
                {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
                {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
                {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
                {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        };

        // For index buffer. (Front is counter-clockwise.)
        const std::vector<uint32_t> indices = {
                0, 2, 1, 2, 0, 3
        };

        default_surface_2d_gou_resources = std::make_shared<SurfaceGpuResources>(vertices, indices);
    }

    static DefaultResource &get_singleton() {
        static DefaultResource singleton;
        return singleton;
    }

    inline void cleanup() {
        default_surface_2d_gou_resources.reset();
    }

    /**
     * Same GPU vertex and index buffers will be shared by all 2D meshes instanced this way.
     * @return
     */
    inline std::shared_ptr<Mesh2d> new_default_mesh_2d() const {
        auto surface = std::make_shared<Surface2d>();
        auto material = std::make_shared<Material2d>();

        auto mesh = std::make_shared<Mesh2d>();
        mesh->surface = surface;
        mesh->surface->set_gpu_resources(default_surface_2d_gou_resources);
        mesh->surface->set_material(material);

        return mesh;
    }

private:
    // Default resources should never be modified.
    std::shared_ptr<SurfaceGpuResources> default_surface_2d_gou_resources;
};

#endif //FLINT_DEFAULT_RESOURCE_H
