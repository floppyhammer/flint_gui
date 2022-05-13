#include "texture_rect.h"

#include "../sub_viewport.h"
#include "../../../core/engine.h"
#include "../../../render/swap_chain.h"
#include "../../../resources/default_resource.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <memory>
#include <chrono>

namespace Flint {
    TextureRect::TextureRect() {
        type = NodeType::TextureRect;

        mesh = DefaultResource::get_singleton().new_default_mesh_2d();
    }

    void TextureRect::set_texture(std::shared_ptr<ImageTexture> p_texture) {
        mesh->surface->get_material()->set_texture(p_texture);

        size.x = (float) p_texture->get_width();
        size.y = (float) p_texture->get_height();
    }

    std::shared_ptr<ImageTexture> TextureRect::get_texture() const {
        return mesh->surface->get_material()->get_texture();
    }

    void TextureRect::update(double dt) {
        update_mvp();
    }

    void TextureRect::draw(VkCommandBuffer p_command_buffer) {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RenderServer::getSingleton().blitGraphicsPipeline;
        VkPipelineLayout pipeline_layout = RenderServer::getSingleton().blitPipelineLayout;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->render_target->blitGraphicsPipeline;
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

    void TextureRect::update_mvp() {
        Node *viewport_node = get_viewport();

        Vec2<uint32_t> viewport_extent;
        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            viewport_extent = viewport->get_extent();
        } else { // Default to swap chain image.
            auto extent = SwapChain::getSingleton().swapChainExtent;
            viewport_extent = Vec2<uint32_t>(extent.width, extent.height);
        }

        // Prepare MVP data. We use this matrix to convert a full-screen to the NodeGui's rect.
        ModelViewProjection mvp{};

        // The actual application order of these matrices is reverse.
        // 4.
        mvp.model = glm::translate(glm::mat4(1.0f),
                                   glm::vec3(position.x / viewport_extent.x * 2.0f,
                                             position.y / viewport_extent.y * 2.0f,
                                             0.0f));
        // 3.
        mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
        // 2.
        mvp.model = glm::scale(mvp.model, glm::vec3(scale.x, scale.y, 1.0f));
        // 1.
        mvp.model = glm::scale(mvp.model,
                               glm::vec3(size.x / viewport_extent.x * 2.0f,
                                         size.y / viewport_extent.y * 2.0f,
                                         1.0f));

        push_constant.model = mvp.model;
    }
}
