#include "model.h"

#include <utility>

#include "../../../common/io.h"
#include "../../../io/obj_importer.h"
#include "../../../render/swap_chain.h"
#include "../../../resources/resource_manager.h"
#include "../sub_viewport.h"

namespace Flint {
Model::Model() {
    type = NodeType::Model;
}

void Model::draw(VkCommandBuffer p_command_buffer) {
    Node *viewport_node = get_viewport();

    VkPipeline pipeline = RenderServer::getSingleton()->meshGraphicsPipeline;
    VkPipelineLayout pipeline_layout = RenderServer::getSingleton()->blitPipelineLayout;

    if (viewport_node) {
        auto viewport = dynamic_cast<SubViewport *>(viewport_node);
        pipeline = viewport->render_target->meshGraphicsPipeline;
    }

    // Upload the model matrix to the GPU via push constants.
    vkCmdPushConstants(
        p_command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MvpPushConstant), &push_constant);

    for (auto &surface : mesh->surfaces) {
        const auto &desc_set = surface->get_material()->get_desc_set();

        VkBuffer vertexBuffers[] = {surface->get_vertex_buffer()};
        RenderServer::getSingleton()->draw_mesh(p_command_buffer,
                                                pipeline,
                                                desc_set->getDescriptorSet(SwapChain::getSingleton()->currentImage),
                                                vertexBuffers,
                                                surface->get_index_buffer(),
                                                surface->get_index_count());
    }
}

void Model::set_mesh(std::shared_ptr<Mesh3d> p_mesh) {
    mesh = std::move(p_mesh);
}
} // namespace Flint
