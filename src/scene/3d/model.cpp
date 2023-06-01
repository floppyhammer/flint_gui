#include "model.h"

#include <utility>

#include "../../common/load_file.h"
#include "../../common/obj_importer.h"
#include "../../render/swap_chain.h"
#include "../../resources/resource_manager.h"
#include "../world.h"
#include "camera3d.h"

namespace Flint {

Model::Model() {
    type = NodeType::Model;
}

void Model::draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) {
    auto world = get_world();
    if (!world) {
        return;
    }

    auto camera = get_world()->current_camera3d;
    if (!camera) {
        return;
    }

    VkPipeline pipeline = RenderServer::get_singleton()->get_pipeline(render_pass, "mesh3d");
    VkPipelineLayout pipeline_layout = RenderServer::get_singleton()->mesh_pipeline_layout;

    // Upload the model matrix to the GPU via push constants.
    vkCmdPushConstants(
        cmd_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MvpPushConstant), &push_constant);

    for (auto &surface : mesh->surfaces) {
        const auto &desc_set = surface->get_material()->get_desc_set();

        VkBuffer vertexBuffers[] = {surface->get_vertex_buffer()};
        RenderServer::get_singleton()->draw_mesh3d(cmd_buffer,
                                                   pipeline,
                                                   desc_set->getDescriptorSet(get_current_image()),
                                                   vertexBuffers,
                                                   surface->get_index_buffer(),
                                                   surface->get_index_count());
    }
}

void Model::set_mesh(std::shared_ptr<Mesh3d> new_mesh) {
    mesh = std::move(new_mesh);
}

void Model::update(double dt) {
    update_mvp();
}

} // namespace Flint
