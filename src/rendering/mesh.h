#ifndef FLINT_MESH_H
#define FLINT_MESH_H

#include "material.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include "glm/glm.hpp"

#include <string>
#include <utility>
#include <vector>
#include <array>

/**
 * Shared by 2D and 3D meshes.
 */
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv; // Texture coordinates.

    bool operator==(const Vertex &other) const {
        return pos == other.pos && color == other.color && uv == other.uv;
    }

    /// Binding info.
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Specify rate at which vertex attributes are pulled from buffers.

        return bindingDescription;
    }

    /// Attributes info.
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, uv);

        return attributeDescriptions;
    }
};

class Mesh {
public:
    Mesh() = default;

    ~Mesh();

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
    virtual void createDescriptorSets() = 0;

    /**
     * Should be called once uniform/texture bindings changed.
     * @dependency Actual resources (buffers, images, image views).
     */
    virtual void updateDescriptorSets(std::shared_ptr<Material>, std::vector<VkBuffer> &uniformBuffers) = 0;

    [[nodiscard]] VkDescriptorSet getDescriptorSet(uint32_t index) const;

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

protected:
    /// A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
    VkDescriptorPool descriptorPool{};

    /// Descriptor sets are allocated from descriptor pool objects.
    std::vector<VkDescriptorSet> descriptorSets;
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

class Mesh2D : public Mesh {
public:
    Mesh2D();

    /**
     * Default quad mesh.
     */
    static std::shared_ptr<Mesh2D> from_default() {
        auto mesh = std::make_shared<Mesh2D>();

        mesh->create_vertex_buffer();

        mesh->create_index_buffer();

        return mesh;
    }

    void createDescriptorPool() override;

    void createDescriptorSets() override;

    void updateDescriptorSets(std::shared_ptr<Material> p_material, std::vector<VkBuffer> &uniformBuffers) override;

private:
    void create_vertex_buffer();

    void create_index_buffer();
};

class Mesh3D : public Mesh {
public:
    Mesh3D();

    void createDescriptorPool() override;

    void createDescriptorSets() override;

    void updateDescriptorSets(std::shared_ptr<Material> p_material, std::vector<VkBuffer> &uniformBuffers) override;
};

#endif //FLINT_MESH_H
