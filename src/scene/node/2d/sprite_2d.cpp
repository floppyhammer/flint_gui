#include "sprite_2d.h"

#include "../sub_viewport.h"
#include "../../../render/mvp_buffer.h"
#include "../../../render/swap_chain.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    Sprite2d::Sprite2d() {
        type = NodeType::Sprite2D;
    }

    void Sprite2d::set_texture(std::shared_ptr<Texture> p_texture) {
        mesh->surface->get_material()->set_texture(p_texture);
    }

    std::shared_ptr<Texture> Sprite2d::get_texture() const {
        return mesh->surface->get_material()->get_texture();
    }

    void Sprite2d::set_mesh(const std::shared_ptr<Mesh2d>& p_mesh) {
        mesh = p_mesh;
    }

    void Sprite2d::set_material(const std::shared_ptr<Material2d>& p_material) {
        mesh->surface->set_material(p_material);
    }

    void Sprite2d::_update(double delta) {
        // Update self.
        update(delta);

        // Update children;
        Node::_update(delta);
    }

    void Sprite2d::update(double delta) {
        update_mvp();
    }

    void Sprite2d::update_mvp() {
        if (mesh == nullptr || mesh->surface == nullptr) {
            return;
        }

        Node *viewport_node = get_viewport();

        Vec2<uint32_t> viewport_extent;
        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            viewport_extent = viewport->get_extent();
        } else { // Default to swap chain image.
            auto extent = SwapChain::getSingleton().swapChainExtent;
            viewport_extent = Vec2<uint32_t>(extent.width, extent.height);
        }

        float sprite_width = mesh->surface->get_material()->get_texture()->width * scale.x;
        float sprite_height = mesh->surface->get_material()->get_texture()->height * scale.y;

        auto global_position = get_global_position();

        // Prepare MVP data. We use this matrix to convert a full-screen to the control's rect.
        ModelViewProjection mvp{};

        // The actual application order of these matrices is reverse.
        // 4.
        mvp.model = glm::translate(glm::mat4(1.0f),
                                   glm::vec3(global_position.x / viewport_extent.x * 2.0f,
                                             global_position.y / viewport_extent.y * 2.0f,
                                             0.0f));
        // 3.
        mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
        // 2.
        mvp.model = glm::scale(mvp.model, glm::vec3(scale.x, scale.y, 1.0f));
        // 1.
        mvp.model = glm::scale(mvp.model,
                               glm::vec3(sprite_width / viewport_extent.x * 2.0f,
                                         sprite_height / viewport_extent.y * 2.0f,
                                         1.0f));

        push_constant.model = mvp.model;
    }

    void Sprite2d::_draw(VkCommandBuffer p_command_buffer) {
        draw(p_command_buffer);

        Node::_draw(p_command_buffer);
    }

    void Sprite2d::draw(VkCommandBuffer p_command_buffer) {
        if (mesh == nullptr) {
            Logger::warn("No valid mesh set for Sprite 2D!");
            return;
        }

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
                mesh->surface->get_material()->get_desc_set()->getDescriptorSet(SwapChain::getSingleton().currentImage),
                vertexBuffers,
                mesh->surface->get_index_buffer(),
                mesh->surface->get_index_count());
    }
}
