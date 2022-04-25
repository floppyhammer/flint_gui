#ifndef FLINT_SURFACE_H
#define FLINT_SURFACE_H

#include "material.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <string>
#include <utility>
#include <vector>
#include <array>

/// Mesh Resources

class Surface {
public:
    Surface() = default;

    ~Surface();

    std::string name;

public:
    /// Vertex buffer.
    VkBuffer vertexBuffer{};
    VkDeviceMemory vertexBufferMemory{};

    /// Index buffer.
    VkBuffer indexBuffer{};
    VkDeviceMemory indexBufferMemory{};
    uint32_t indices_count = 0;
};

struct Surface2dPushConstant {
    glm::mat4 model;
};

// TODO: We might need to push model, view and projection separately.
struct Surface3dPushConstant {
    glm::mat4 mvp;
};

class Surface2d : public Surface {
public:
    Surface2d() = default;

    /**
     * Default quad surface.
     */
    static std::shared_ptr<Surface2d> from_default() {
        auto surface = std::make_shared<Surface2d>();

        surface->create_vertex_buffer();

        surface->create_index_buffer();

        return surface;
    }

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

    void set_material(const std::shared_ptr<Material2d> &p_material);

    std::shared_ptr<Material2d> get_material() const;

private:
    void create_vertex_buffer();

    void create_index_buffer();

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

    std::shared_ptr<Material3d> material;
};

#endif //FLINT_SURFACE_H
