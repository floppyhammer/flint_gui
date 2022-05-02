#include "texture_rect.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "../../../core/engine.h"
#include "../../../render/swap_chain.h"
#include "../sub_viewport.h"

#include <memory>
#include <chrono>

namespace Flint {
    TextureRect::TextureRect() {
        type = NodeType::TextureRect;
    }

    void TextureRect::set_texture(std::shared_ptr<Texture> p_texture) {
        mesh->surface->get_material()->set_texture(p_texture);

        size.x = (float) p_texture->width;
        size.y = (float) p_texture->height;
    }

    std::shared_ptr<Texture> TextureRect::get_texture() const {
        return mesh->surface->get_material()->get_texture();
    }

    void TextureRect::_update(double delta) {
        // Update self.
        NodeGui::update(delta);

        // Update children.
        Node::_update(delta);
    }

    void TextureRect::update(double delta) {
    }

    void TextureRect::_draw(VkCommandBuffer p_command_buffer) {
        draw(p_command_buffer);

        Node::_draw(p_command_buffer);
    }

    void TextureRect::draw(VkCommandBuffer p_command_buffer) {
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

        // Unlike Sprite 2D, Texture Rect should not support custom mesh.
        RenderServer::getSingleton().blit(
                p_command_buffer,
                pipeline,
                mesh->surface->get_material()->get_desc_set()->getDescriptorSet(SwapChain::getSingleton().currentImage));
    }
}
