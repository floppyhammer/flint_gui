#ifndef FLINT_DEFAULT_RESOURCE_H
#define FLINT_DEFAULT_RESOURCE_H

#include "surface.h"
#include "mesh.h"

namespace Flint {
    class DefaultResource {
    public:
        DefaultResource() {
            // Default vertex and index data for 2D quad surface.
            {
                const std::vector<Vertex> vertices = {
                        {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
                        {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
                        {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
                        {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
                };

                // Front is counter-clockwise.
                const std::vector<uint32_t> indices = {
                        0, 2, 1,
                        2, 0, 3,
                };

                default_surface_2d_gpu_resources = std::make_shared<VertexGpuResources<Vertex>>(vertices, indices);
            }

            // Default vertex and index data for Skybox.
            {
                const std::vector<SkyboxVertex> skybox_vertices = {
                        {{-1.0f, -1.0f, -1.0f}},
                        {{1.0f, -1.0f, -1.0f}},
                        {{1.0f, 1.0f, -1.0f}},
                        {{-1.0f, 1.0f, -1.0f}},
                        {{-1.0f, -1.0f, 1.0f}},
                        {{1.0f, -1.0f, 1.0f}},
                        {{1.0f, 1.0f, 1.0f}},
                        {{-1.0f, 1.0f, 1.0f}},
                };

                const std::vector<uint32_t> skybox_indices = {
                        0, 1, 2,
                        2, 3, 0,
                        4, 6, 5,
                        6, 4, 7,
                        2, 6, 7,
                        2, 7, 3,
                        1, 5, 6,
                        1, 6, 2,
                        3, 7, 0,
                        4, 0, 7,
                        5, 1, 4,
                        4, 1, 0,
                };

                default_skybox_gpu_resources = std::make_shared<VertexGpuResources<SkyboxVertex>>(skybox_vertices, skybox_indices);
            }
        }

        static DefaultResource *get_singleton() {
            static DefaultResource singleton;
            return &singleton;
        }

        inline std::shared_ptr<VertexGpuResources<Vertex>> get_default_surface_2d_gpu_resources() {
            return default_surface_2d_gpu_resources;
        }

        inline std::shared_ptr<VertexGpuResources<SkyboxVertex>> get_default_skybox_gpu_resources() {
            return default_skybox_gpu_resources;
        }

        inline void cleanup() {
            default_surface_2d_gpu_resources.reset();
            default_skybox_gpu_resources.reset();
        }

        /**
         * Same vertex and index buffers will be shared by all default 2D meshes.
         * @return Mesh2d
         */
        inline std::shared_ptr<Mesh2d> new_default_mesh_2d() const {
            auto surface = std::make_shared<Surface2d>();
            auto material = std::make_shared<Material2d>();

            auto mesh = std::make_shared<Mesh2d>();
            mesh->surface = surface;
            mesh->surface->set_gpu_resources(default_surface_2d_gpu_resources);
            mesh->surface->set_material(material);

            return mesh;
        }

    private:
        // Default resources should never be modified.
        std::shared_ptr<VertexGpuResources<Vertex>> default_surface_2d_gpu_resources;

        std::shared_ptr<VertexGpuResources<SkyboxVertex>> default_skybox_gpu_resources;
    };
}

#endif //FLINT_DEFAULT_RESOURCE_H
