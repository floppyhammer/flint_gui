#include "sub_viewport_container.h"

#include "../sub_viewport.h"
#include "../../../render/swap_chain.h"
#include "../../../resources/default_resource.h"

#include <utility>
#include <array>

namespace Flint {
    SubViewportContainer::SubViewportContainer() {
        type = NodeType::SubViewportContainer;

        mesh = DefaultResource::get_singleton().new_default_mesh_2d();
    }

    void SubViewportContainer::set_viewport(std::shared_ptr<SubViewport> p_viewport) {
        viewport = std::move(p_viewport);

        mesh->surface->get_material()->set_texture(viewport->get_texture());
    }

    void SubViewportContainer::update(double dt) {
        Control::update(dt);
    }

    void SubViewportContainer::propagate_draw(VkCommandBuffer p_command_buffer) {
        // Don't call Node::_draw(), so we can break the recursive calling
        // to call the sub-viewport draw function below specifically.
        // Also, we can't interrupt our previous render pass.

        auto sub_viewport_command_buffer = RenderServer::getSingleton().beginSingleTimeCommands();

        // Start sub-viewport render pass.
        if (viewport != nullptr) {
            viewport->propagate_draw(sub_viewport_command_buffer);
        }

        RenderServer::getSingleton().endSingleTimeCommands(sub_viewport_command_buffer);

        // Now draw the rendered sub-viewport texture.
        draw(p_command_buffer);
    }

    void SubViewportContainer::draw(VkCommandBuffer p_command_buffer) {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RenderServer::getSingleton().blitGraphicsPipeline;
        VkPipelineLayout pipeline_layout = RenderServer::getSingleton().blitPipelineLayout;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->viewport->blitGraphicsPipeline;
        }

        // Upload the model matrix to the GPU via push constants.
        vkCmdPushConstants(p_command_buffer, pipeline_layout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(Surface2dPushConstant), &push_constant);

        VkBuffer vertexBuffers[] = {mesh->surface->get_vertex_buffer()};
        RenderServer::getSingleton().blit(
                p_command_buffer,
                pipeline,
                mesh->surface->get_material()->get_desc_set()->getDescriptorSet(SwapChain::getSingleton().currentImage));
    }
}
