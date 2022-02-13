//
// Created by tannh on 2/13/2022.
//

#include "mesh.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/hash.hpp"

#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"

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

void Mesh::loadFile() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error(warn + err);
    }

    // Use a map or unordered_map to keep track of the unique vertices and respective indices.
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    // Iterate over the vertices and dump them straight into our vertices vector.
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
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
    }
}
