#include "model.h"

#include "../../../common/io.h"
#include "../../../io/obj_importer.h"
#include "../../../resources/resource_manager.h"
#include "../../../render/swap_chain.h"
#include "../sub_viewport.h"

#include <utility>

namespace Flint {
    Model::Model() {
        type = NodeType::Model;
    }

    void Model::_update(double delta) {
        // Update self.
        Node3D::update(delta);

        // Update children.
        Node::_update(delta);
    }

    void Model::_draw(VkCommandBuffer p_command_buffer) {
        draw(p_command_buffer);

        Node::_draw(p_command_buffer);
    }

    void Model::draw(VkCommandBuffer p_command_buffer) {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RenderServer::getSingleton().meshGraphicsPipeline;
        VkPipelineLayout pipeline_layout = RenderServer::getSingleton().blitPipelineLayout;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->viewport->meshGraphicsPipeline;
        }

        // Upload the model matrix to the GPU via push constants.
        vkCmdPushConstants(p_command_buffer, pipeline_layout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(Surface3dPushConstant), &push_constant);

        for (auto &surface: mesh->surfaces) {
            const auto &desc_set = surface->get_material()->get_desc_set();

            VkBuffer vertexBuffers[] = {surface->vertexBuffer};
            RenderServer::getSingleton().draw_mesh(
                    p_command_buffer,
                    pipeline,
                    desc_set->getDescriptorSet(SwapChain::getSingleton().currentImage),
                    vertexBuffers,
                    surface->indexBuffer,
                    surface->indices_count);
        }
    }

    void Model::set_mesh(std::shared_ptr<Mesh3d> p_mesh) {
        mesh = std::move(p_mesh);
    }
}
