#ifndef FLINT_MESH_H
#define FLINT_MESH_H

#include "material.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <string>
#include <utility>
#include <vector>
#include <array>

/// Mesh Resources

class Mesh : public Resource {
public:
    Mesh() = default;

    ~Mesh();

public:
    std::string name;

    /// Material index in a material vector/list/set as different meshes can use the same material.
    int32_t material_id = -1;

    /// Vertex buffer.
    VkBuffer vertexBuffer{};
    VkDeviceMemory vertexBufferMemory{};

    /// Index buffer.
    VkBuffer indexBuffer{};
    VkDeviceMemory indexBufferMemory{};
    uint32_t indices_count = 0;
};

/**
 * Do not use the base class directly.
 */
class MeshDescSet {
public:
    ~MeshDescSet();

    /**
     * A descriptor pool is used to allocate descriptor sets of some layout for use in a shader.
     * Do this before creating descriptor sets.
     * @dependency None.
     */
    virtual void createDescriptorPool() = 0;

    /**
     * Allocate descriptor sets in the pool.
     * @dependency Descriptor pool, descriptor set layout.
     */
    virtual void createDescriptorSet() = 0;

    /**
     * Should be called once uniform/texture bindings changed.
     * @dependency Actual resources (buffers, images, image views).
     */
    //virtual void updateDescriptorSet(std::shared_ptr<Material>, std::vector<VkBuffer> &uniformBuffers) = 0;

    [[nodiscard]] VkDescriptorSet getDescriptorSet(uint32_t index) const;

protected:
    /// A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
    VkDescriptorPool descriptorPool{};

    /// Descriptor sets are allocated from descriptor pool objects.
    std::vector<VkDescriptorSet> descriptorSets;
};

class Mesh2dDescSet : public MeshDescSet {
public:
    Mesh2dDescSet();

    void createDescriptorPool() override;

    void createDescriptorSet() override;

    void updateDescriptorSet(const std::shared_ptr<Material>& p_material);
};

class Mesh3dDescSet : public MeshDescSet {
public:
    Mesh3dDescSet();

    void createDescriptorPool() override;

    void createDescriptorSet() override;

    void updateDescriptorSet(const std::shared_ptr<Material>& p_material);
};

// Default vertices and indices data for 2D quad mesh.
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

struct Mesh2dPushConstant {
    glm::mat4 model;
};

// TODO: We might need to push model, view and projection separately.
struct Mesh3dPushConstant {
    glm::mat4 mvp;
};

// TODO: Simple quad meshes should all share the same vertex and index buffers.
class Mesh2D : public Mesh {
public:
    Mesh2D() = default;

    /**
     * Default quad mesh.
     */
    static std::shared_ptr<Mesh2D> from_default() {
        auto mesh = std::make_shared<Mesh2D>();

        mesh->create_vertex_buffer();

        mesh->create_index_buffer();

        return mesh;
    }

private:
    void create_vertex_buffer();

    void create_index_buffer();
};

class Mesh3D : public Mesh {
public:
    Mesh3D() = default;

    static std::shared_ptr<Mesh3D> from_plane() {
        return std::make_shared<Mesh3D>();
    }

    static std::shared_ptr<Mesh3D> from_cube() {
        return std::make_shared<Mesh3D>();
    }

    static std::shared_ptr<Mesh3D> from_sphere() {
        return std::make_shared<Mesh3D>();
    }
};

#endif //FLINT_MESH_H
