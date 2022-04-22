#include "obj_importer.h"

#include "../common/io.h"
#include "../common/logger.h"

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
                   (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
}

namespace Flint {
    void ObjImporter::load_file(const std::string &filename,
                                std::vector<std::shared_ptr<Mesh3d>> &meshes,
                                std::vector<std::shared_ptr<Mesh3dDescSet>> &desc_sets,
                                std::vector<std::shared_ptr<Material3d>> &materials) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> obj_materials;
        std::string warn, err;

        // Get file base.
        std::string file_directory;
        split_filename(filename, file_directory);

        if (!tinyobj::LoadObj(&attrib, &shapes, &obj_materials,
                              &warn, &err, filename.c_str(), file_directory.c_str())) {
            throw std::runtime_error(warn + err);
        }

        // Load materials.
        if (obj_materials.empty()) {
            Logger::warn("No material found in the .mtl file or no .mtl file found at " + file_directory,
                         "OBJ Importer");

            // Default material.
            materials.push_back(Material3d::from_default());
        } else {
            for (const auto &obj_material: obj_materials) {
                auto material = std::make_shared<Material3d>();
                material->name = obj_material.name;
                material->diffuse_texture = Texture::from_file(file_directory + "/" + obj_material.diffuse_texname);
                materials.push_back(material);
            }
        }

        // Iterate over the vertices and dump them straight into our vertices vector.
        for (const auto &shape: shapes) {
            auto mesh = std::make_shared<Mesh3d>();

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

                vertex.uv = {
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

            RenderServer::getSingleton().createVertexBuffer(vertices, mesh->vertexBuffer, mesh->vertexBufferMemory);
            RenderServer::getSingleton().createIndexBuffer(indices, mesh->indexBuffer, mesh->indexBufferMemory);

            auto desc_set = std::make_shared<Mesh3dDescSet>();
            if (mesh->material_id > 0 && mesh->material_id < materials.size()) {
                desc_set->updateDescriptorSet(materials[mesh->material_id]);
            } else {
                mesh->material_id = 0;
                desc_set->updateDescriptorSet(materials[0]);
                //throw std::runtime_error("Invalid material id!");
            }

            meshes.push_back(mesh);
            desc_sets.push_back(desc_set);
        }
    }
}
