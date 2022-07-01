#include "sprite_render_system.h"

#include "../components/components.h"
#include "../../../render/swap_chain.h"
#include "../../../resources/surface.h"
#include "../../../render/mvp.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    void SpriteGuiRenderSystem::update(const std::weak_ptr<Coordinator>& p_coordinator, double dt) {
        auto coordinator = p_coordinator.lock();

        for (auto const &entity: entities) {
            auto &sprite = coordinator->get_component<Sprite2dComponent>(entity);
            auto &transform = coordinator->get_component<TransformGuiComponent>(entity);

            // Default to swap chain image.
            auto extent = SwapChain::getSingleton()->swapChainExtent;
            Vec2<uint32_t> viewport_extent = Vec2<uint32_t>(extent.width, extent.height);

            // Prepare MVP data. We use this matrix to convert a full-screen to the control's rect.
            ModelViewProjection mvp{};

            // The actual application order of these matrices is reverse.
            // 4.
            mvp.model = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(transform.position.x / viewport_extent.x * 2.0f,
                                                 transform.position.y / viewport_extent.y * 2.0f,
                                                 0.0f));
            // 3.
            mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
            // 2.
            mvp.model = glm::scale(mvp.model, glm::vec3(transform.scale.x, transform.scale.y, 1.0f));
            // 1.
            mvp.model = glm::scale(mvp.model,
                                   glm::vec3(transform.size.x / viewport_extent.x * 2.0f,
                                             transform.size.y / viewport_extent.y * 2.0f,
                                             1.0f));

            sprite.push_constant.mvp = mvp.model;
        }
    }

    void SpriteGuiRenderSystem::draw(const std::weak_ptr<Coordinator>& p_coordinator, VkCommandBuffer command_buffer) {
        auto coordinator = p_coordinator.lock();

        for (auto const &entity: entities) {
            auto &sprite = coordinator->get_component<Sprite2dComponent>(entity);

            VkPipeline pipeline = RenderServer::getSingleton()->blitGraphicsPipeline;
            VkPipelineLayout pipeline_layout = RenderServer::getSingleton()->blitPipelineLayout;

            // Upload the model matrix to the GPU via push constants.
            vkCmdPushConstants(command_buffer, pipeline_layout,
                               VK_SHADER_STAGE_VERTEX_BIT, 0,
                               sizeof(MvpPushConstant), &sprite.push_constant.mvp);

            VkBuffer vertexBuffers[] = {sprite.mesh->surface->get_vertex_buffer()};
            RenderServer::getSingleton()->draw_mesh_2d(
                    command_buffer,
                    pipeline,
                    sprite.mesh->surface->get_material()->get_desc_set()->getDescriptorSet(SwapChain::getSingleton()->currentImage),
                    vertexBuffers,
                    sprite.mesh->surface->get_index_buffer(),
                    sprite.mesh->surface->get_index_count());
        }
    }

    void Sprite2dRenderSystem::update(const std::weak_ptr<Coordinator>& p_coordinator, double dt) {
        auto coordinator = p_coordinator.lock();

        for (auto const &entity: entities) {
            auto &sprite = coordinator->get_component<Sprite2dComponent>(entity);
            auto &transform = coordinator->get_component<Transform2dComponent>(entity);
            auto &sort_z = coordinator->get_component<ZSort2d>(entity);

            auto texture = sprite.mesh->surface->get_material()->get_texture();
            if (texture == nullptr) continue;

            float sprite_width = texture->get_width() * transform.scale.x;
            float sprite_height = texture->get_height() * transform.scale.y;

            // Default to swap chain image.
            auto extent = SwapChain::getSingleton()->swapChainExtent;
            Vec2<uint32_t> viewport_extent = Vec2<uint32_t>(extent.width, extent.height);

            // Prepare MVP data. We use this matrix to convert a full-screen to the control's rect.
            ModelViewProjection mvp{};

            // The actual application order of these matrices is reverse.
            // 4.
            mvp.model = glm::translate(glm::mat4(1.0f),
                                       glm::vec3((transform.position.x) / viewport_extent.x * 2.0f,
                                                 (transform.position.y) / viewport_extent.y * 2.0f,
                                                 sort_z.z));
            // 3.
            mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
            // 2.
            mvp.model = glm::scale(mvp.model, glm::vec3(transform.scale.x, transform.scale.y, 1.0f));
            // 1.
            mvp.model = glm::scale(mvp.model,
                                   glm::vec3(sprite_width / viewport_extent.x * 2.0f,
                                             sprite_height / viewport_extent.y * 2.0f,
                                             1.0f));

            sprite.push_constant.mvp = mvp.model;
        }
    }

    void Sprite2dRenderSystem::draw(const std::weak_ptr<Coordinator>& p_coordinator, VkCommandBuffer command_buffer) {
        auto coordinator = p_coordinator.lock();

        for (auto const &entity: entities) {
            auto &sprite = coordinator->get_component<Sprite2dComponent>(entity);

            auto texture = sprite.mesh->surface->get_material()->get_texture();
            if (texture == nullptr) continue;

            VkPipeline pipeline = RenderServer::getSingleton()->blitGraphicsPipeline;
            VkPipelineLayout pipeline_layout = RenderServer::getSingleton()->blitPipelineLayout;

            // Upload the model matrix to the GPU via push constants.
            vkCmdPushConstants(command_buffer, pipeline_layout,
                               VK_SHADER_STAGE_VERTEX_BIT, 0,
                               sizeof(MvpPushConstant), &sprite.push_constant.mvp);

            VkBuffer vertexBuffers[] = {sprite.mesh->surface->get_vertex_buffer()};
            RenderServer::getSingleton()->draw_mesh_2d(
                    command_buffer,
                    pipeline,
                    sprite.mesh->surface->get_material()->get_desc_set()->getDescriptorSet(SwapChain::getSingleton()->currentImage),
                    vertexBuffers,
                    sprite.mesh->surface->get_index_buffer(),
                    sprite.mesh->surface->get_index_count());
        }
    }
}
