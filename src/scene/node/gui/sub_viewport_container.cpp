#include "sub_viewport_container.h"

#include "../sub_viewport.h"
#include "../../../render/swap_chain.h"

#include <utility>
#include <array>

namespace Flint {
    SubViewportContainer::SubViewportContainer() {
        type = NodeType::SubViewportContainer;

        init_default_mesh();
    }

    void SubViewportContainer::set_viewport(std::shared_ptr<SubViewport> p_viewport) {
        viewport = std::move(p_viewport);

        material->texture = viewport->get_texture();

        //mesh->updateDescriptorSets(material, mvp_buffer->uniform_buffers);
    }

    void SubViewportContainer::_update(double delta) {
        // Update self.
        NodeGui::update(delta);

        // Update children.
        Node::_update(delta);
    }

    void SubViewportContainer::update(double delta) {
    }

    void SubViewportContainer::_draw(VkCommandBuffer p_command_buffer) {
        // Don't call Node::_draw(), so we can break the recursive calling
        // to call the sub-viewport draw function below specifically.
        // Also, we can't interrupt our previous render pass.

        auto sub_viewport_command_buffer = RenderServer::getSingleton().beginSingleTimeCommands();

        // Start sub-viewport render pass.
        if (viewport != nullptr) {
            viewport->_draw(sub_viewport_command_buffer);
        }

        RenderServer::getSingleton().endSingleTimeCommands(sub_viewport_command_buffer);

        // Now draw the rendered sub-viewport texture.
        draw(p_command_buffer);
    }

    void SubViewportContainer::draw(VkCommandBuffer p_command_buffer) {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RenderServer::getSingleton().blitGraphicsPipeline;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->viewport->blitGraphicsPipeline;
        }

        VkBuffer vertexBuffers[] = {mesh->vertexBuffer};
//        RenderServer::getSingleton().blit(
//                p_command_buffer,
//                pipeline,
//                mesh->getDescriptorSet(SwapChain::getSingleton().currentImage),
//                vertexBuffers,
//                mesh->indexBuffer,
//                mesh->indices_count);
    }
}
