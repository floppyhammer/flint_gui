#ifndef FLINT_SURFACE_H
#define FLINT_SURFACE_H

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "../render/mesh.h"
#include "../render/vertex.h"
#include "material.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Flint {

class Surface {
public:
    Surface() = default;

    std::string name;

    VkBuffer get_vertex_buffer();

    VkBuffer get_index_buffer();

    uint32_t get_index_count();

    void set_gpu_resources(std::shared_ptr<MeshGpu<Vertex>> new_gpu_resources);

private:
    std::shared_ptr<MeshGpu<Vertex>> gpu_resources;
};

class Surface2d : public Surface {
public:
    Surface2d() = default;

    /**
     * Default quad surface.
     */
    static std::shared_ptr<Surface2d> from_default();

    void set_material(const std::shared_ptr<Material2d> &p_material);

    std::shared_ptr<Material2d> get_material() const;

private:
    std::shared_ptr<Material2d> material;
};

class Surface3d : public Surface {
public:
    Surface3d() = default;

    static std::shared_ptr<Surface3d> from_plane() {
        return std::make_shared<Surface3d>();
    }

    static std::shared_ptr<Surface3d> from_cube() {
        return std::make_shared<Surface3d>();
    }

    static std::shared_ptr<Surface3d> from_sphere() {
        return std::make_shared<Surface3d>();
    }

    void set_material(const std::shared_ptr<Material3d> &_material);

    std::shared_ptr<Material3d> get_material() const;

private:
    std::shared_ptr<Material3d> material;
};

} // namespace Flint

#endif // FLINT_SURFACE_H
