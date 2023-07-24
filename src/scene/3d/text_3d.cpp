#include "text_3d.h"

#include "../../resources/font_mesh.h"
#include "../world.h"
#include "mesh_instance_3d.h"

namespace Flint {

Text3d::Text3d() {
    type = NodeType::Text3d;

    font_mesh = std::make_shared<FontMesh>("../assets/fonts/Arial Unicode MS Font.ttf");

    instance = std::make_shared<MeshInstance3d>();
    add_child(instance);
}

void Text3d::draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) {
    auto world = get_world();
    if (!world) {
        return;
    }

    auto camera = get_world()->current_camera3d;
    if (!camera) {
        return;
    }

    //    instance->propagate_draw(render_pass, cmd_buffer);
}

void Text3d::update(double dt) {
    rotation.x += dt;

    update_mvp();

    //    instance->propagate_update(dt);
}

void Text3d::set_text(std::string new_text) {
    if (text == new_text) {
        return;
    }

    text = new_text;

    auto material = std::make_shared<Material3d>();
    material->set_diffuse_texture(ImageTexture::from_empty({2, 2}, VK_FORMAT_R8G8B8A8_UNORM));

    std::vector<std::shared_ptr<Surface3d>> surfaces;
    for (auto& c : text) {
        auto surface = std::make_shared<Surface3d>();
        auto mesh_cpu = font_mesh->create_mesh_from_glyph(c);
        auto mesh_gpu = std::make_shared<MeshGpu<Vertex>>();
        mesh_gpu->upload(mesh_cpu);

        surface->set_gpu_resources(mesh_gpu);
        surface->set_material(material);

        surfaces.push_back(surface);
    }

    instance->set_surfaces(surfaces);
}

} // namespace Flint
