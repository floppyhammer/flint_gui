#include "obj_importer.h"

#include "../resources/resource_manager.h"
#include "load_file.h"
#include "utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <unordered_map>

namespace std {
template <>
struct hash<Flint::Vertex> {
    size_t operator()(Flint::Vertex const &vertex) const {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
               (hash<glm::vec2>()(vertex.uv) << 1);
    }
};
} // namespace std

namespace Flint::Utils {

void ObjImporter::load_file(const std::string &path, std::vector<std::shared_ptr<Surface3d>> &surfaces) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> obj_materials;
    std::string warn, err;

    // Get the directory the file is in.
    std::string file_directory;
    split_path(path, file_directory);

    if (!tinyobj::LoadObj(&attrib, &shapes, &obj_materials, &warn, &err, path.c_str(), file_directory.c_str())) {
        throw std::runtime_error(warn + err);
    }

    // Load materials.
    std::vector<std::shared_ptr<Material3d>> materials;
    if (obj_materials.empty()) {
        Logger::warn("No material found in the .mtl file or no .mtl file found at " + file_directory, "OBJ Importer");
    } else {
        for (const auto &obj_material : obj_materials) {
            auto material = std::make_shared<Material3d>();
            material->name = obj_material.name;
            material->set_diffuse_texture(ResourceManager::get_singleton()->load<ImageTexture>(
                file_directory + "/" + obj_material.diffuse_texname));
            materials.push_back(material);
        }
    }

    // Iterate over the vertices and dump them straight into our vertices vector.
    for (const auto &shape : shapes) {
        auto surface = std::make_shared<Surface3d>();

        surface->name = shape.name;
        int32_t material_id = shape.mesh.material_ids[0];

        // Staging vectors.
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // Use a map or unordered_map to keep track of the unique vertices and respective indices.
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto &index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                          attrib.vertices[3 * index.vertex_index + 1],
                          attrib.vertices[3 * index.vertex_index + 2]};

            vertex.uv = {attrib.texcoords[2 * index.texcoord_index + 0],
                         // Flip the vertical component of the texture coordinates.
                         1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

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

        surface->set_gpu_resources(std::make_shared<VertexGpuResources<Vertex>>(vertices, indices));

        if (material_id >= 0 && material_id < materials.size()) {
            surface->set_material(materials[material_id]);
        }

        surfaces.push_back(surface);
    }
}

} // namespace Flint::Utils
