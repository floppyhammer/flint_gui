#include "surface.h"

#include "default_resource.h"

namespace Flint {
std::shared_ptr<Material2d> Surface2d::get_material() const {
    return material;
}

void Surface2d::set_material(const std::shared_ptr<Material2d> &p_material) {
    material = p_material;
}

std::shared_ptr<Surface2d> Surface2d::from_default() {
    auto surface = std::make_shared<Surface2d>();

    return surface;
}

void Surface3d::set_material(const std::shared_ptr<Material3d> &p_material) {
    material = p_material;
}

std::shared_ptr<Material3d> Surface3d::get_material() const {
    return material;
}

VkBuffer Surface::get_vertex_buffer() {
    if (gpu_resources == nullptr) return nullptr;

    return gpu_resources->get_vertex_buffer();
}

VkBuffer Surface::get_index_buffer() {
    if (gpu_resources == nullptr) return nullptr;

    return gpu_resources->get_index_buffer();
}

uint32_t Surface::get_index_count() {
    if (gpu_resources == nullptr) return 0;

    return gpu_resources->get_index_count();
}

void Surface::set_gpu_resources(std::shared_ptr<VertexGpuResources<Vertex>> p_gpu_resources) {
    gpu_resources = p_gpu_resources;
}
} // namespace Flint
