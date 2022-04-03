#include "model.h"

#include "../../../common/io.h"
#include "../../../rendering/swap_chain.h"
#include "../sub_viewport.h"

#include <utility>

#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/hash.hpp"

#include <unordered_map>

namespace std {
    template<>
    struct hash<Vertex> {
        size_t operator()(Vertex const &vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace Flint {
    const std::string MODEL_NAME = "../res/viking_room/viking_room.obj";

    MeshInstance3D::MeshInstance3D() {
        type = NodeType::MeshInstance3D;

        createUniformBuffers();

        // Load model.
        loadFile(MODEL_NAME);
    }

    MeshInstance3D::~MeshInstance3D() {
        auto device = Device::getSingleton().device;
        auto swapChainImages = SwapChain::getSingleton().swapChainImages;

        // Clean up uniform buffers.
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }
    }

    void MeshInstance3D::_update(double delta) {
        Node3D::update(delta);

        Node::_update(delta);
    }

    void MeshInstance3D::_draw(VkCommandBuffer p_command_buffer) {
        draw(p_command_buffer);

        Node::_draw(p_command_buffer);
    }

    void MeshInstance3D::draw(VkCommandBuffer p_command_buffer) {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RS::getSingleton().meshGraphicsPipeline;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->meshGraphicsPipeline;
        }

        for (const auto &mesh: meshes) {
            VkBuffer vertexBuffers[] = {mesh->vertexBuffer};
            RS::getSingleton().draw_mesh(
                    p_command_buffer,
                    pipeline,
                    mesh->getDescriptorSet(SwapChain::getSingleton().currentImage),
                    vertexBuffers,
                    mesh->indexBuffer,
                    mesh->indices_count);
        }
    }

    void MeshInstance3D::loadFile(const std::string &filename) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> obj_materials;
        std::string warn, err;

        // Get file base.
        std::string file_directory;
        split_filename(filename, file_directory);

        if (!tinyobj::LoadObj(&attrib, &shapes, &obj_materials,
                              &warn, &err,
                              filename.c_str(), file_directory.c_str())) {
            throw std::runtime_error(warn + err);
        }

        // Load materials.
        for (const auto &obj_material: obj_materials) {
            Material material;
            material.name = obj_material.name;
            material.diffuse_texture = Texture::from_file(file_directory + "/" + obj_material.diffuse_texname);
            materials.push_back(material);
        }

        // Iterate over the vertices and dump them straight into our vertices vector.
        for (const auto &shape: shapes) {
            auto mesh = std::make_shared<Mesh>();

            mesh->name = shape.name;
            mesh->material_id = shape.mesh.material_ids[0];

            // Staging vectors.
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            // Use a map or unordered_map to keep track of the unique vertices and respective indices.
            std::unordered_map<Vertex, uint32_t> uniqueVertices{};

            for (const auto &index: shape.mesh.indices) {
                Vertex vertex{};

                vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        // Flip the vertical component of the texture coordinates.
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = {1.0f, 1.0f, 1.0f};

                // Every time we read a vertex from the OBJ file, we check if we've
                // already seen a vertex with the exact same position and texture
                // coordinates before. If not, we add it to vertices and store its
                // index in the uniqueVertices container. After that we add the
                // index of the new vertex to indices. If we've seen the exact same
                // vertex before, then we look up its index in uniqueVertices and
                // store that index in indices.
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }

            mesh->indices_count = indices.size();

            Node3D::createVertexBuffer(vertices, mesh->vertexBuffer, mesh->vertexBufferMemory);
            Node3D::createIndexBuffer(indices, mesh->indexBuffer, mesh->indexBufferMemory);

            if (mesh->material_id < materials.size()) {
                mesh->updateDescriptorSets(materials[mesh->material_id], uniformBuffers);
            } else {
                throw std::runtime_error("Invalid material id!");
            }

            meshes.push_back(mesh);
        }
    }
}
