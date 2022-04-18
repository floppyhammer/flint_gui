#include "sprite_render_system.h"

#include "../components/components.h"
#include "../coordinator.h"
#include "../../../render/swap_chain.h"
#include "../../../resources/mesh.h"
#include "../../../render/mvp_buffer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    void SpriteGuiRenderSystem::update() {
        for (auto const &entity: entities) {
            auto coordinator = Coordinator::get_singleton();

            auto &sprite = coordinator.get_component<Sprite2dComponent>(entity);
            auto &transform = coordinator.get_component<TransformGuiComponent>(entity);
            auto &mvp_component = coordinator.get_component<MvpComponent>(entity);

            // Default to swap chain image.
            auto extent = SwapChain::getSingleton().swapChainExtent;
            Vec2<uint32_t> viewport_extent = Vec2<uint32_t>(extent.width, extent.height);

            // Prepare UBO data. We use this matrix to convert a full-screen to the control's rect.
            UniformBufferObject ubo{};

            // The actual application order of these matrices is reverse.
            // 4.
            ubo.model = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(transform.position.x / viewport_extent.x * 2.0f,
                                                 transform.position.y / viewport_extent.y * 2.0f,
                                                 0.0f));
            // 3.
            ubo.model = glm::translate(ubo.model, glm::vec3(-1.0, -1.0, 0.0f));
            // 2.
            ubo.model = glm::scale(ubo.model, glm::vec3(transform.scale.x, transform.scale.y, 1.0f));
            // 1.
            ubo.model = glm::scale(ubo.model,
                                   glm::vec3(transform.size.x / viewport_extent.x * 2.0f,
                                             transform.size.y / viewport_extent.y * 2.0f,
                                             1.0f));

            mvp_component.mvp_buffer->update_uniform_buffer(ubo);
        }
    }

    void SpriteGuiRenderSystem::draw(VkCommandBuffer command_buffer) {
        auto coordinator = Coordinator::get_singleton();

        for (auto const &entity: entities) {
            auto &sprite = coordinator.get_component<Sprite2dComponent>(entity);

            VkPipeline pipeline = RenderServer::getSingleton().blitGraphicsPipeline;

            VkBuffer vertexBuffers[] = {sprite.mesh->vertexBuffer};
            RenderServer::getSingleton().blit(
                    command_buffer,
                    pipeline,
                    sprite.mesh->getDescriptorSet(SwapChain::getSingleton().currentImage),
                    vertexBuffers,
                    sprite.mesh->indexBuffer,
                    sprite.mesh->indices_count);
        }
    }

    void Sprite2dRenderSystem::update() {
        auto coordinator = Coordinator::get_singleton();

        for (auto const &entity: entities) {
            auto &sprite = coordinator.get_component<Sprite2dComponent>(entity);
            auto &transform = coordinator.get_component<Transform2dComponent>(entity);
            auto &mvp_component = coordinator.get_component<MvpComponent>(entity);

            float sprite_width = sprite.material->texture->width * transform.scale.x;
            float sprite_height = sprite.material->texture->height * transform.scale.y;

            // Default to swap chain image.
            auto extent = SwapChain::getSingleton().swapChainExtent;
            Vec2<uint32_t> viewport_extent = Vec2<uint32_t>(extent.width, extent.height);

            // Prepare UBO data. We use this matrix to convert a full-screen to the control's rect.
            UniformBufferObject ubo{};

            // The actual application order of these matrices is reverse.
            // 4.
            ubo.model = glm::translate(glm::mat4(1.0f),
                                       glm::vec3((transform.position.x) / viewport_extent.x * 2.0f,
                                                 (transform.position.y) / viewport_extent.y * 2.0f,
                                                 0.0f));
            // 3.
            ubo.model = glm::translate(ubo.model, glm::vec3(-1.0, -1.0, 0.0f));
            // 2.
            ubo.model = glm::scale(ubo.model, glm::vec3(transform.scale.x, transform.scale.y, 1.0f));
            // 1.
            ubo.model = glm::scale(ubo.model,
                                   glm::vec3(sprite_width / viewport_extent.x * 2.0f,
                                             sprite_height / viewport_extent.y * 2.0f,
                                             1.0f));

            mvp_component.mvp_buffer->update_uniform_buffer(ubo);
        }
    }

    void Sprite2dRenderSystem::draw(VkCommandBuffer command_buffer) {
        auto coordinator = Coordinator::get_singleton();

        for (auto const &entity: entities) {
            auto &sprite = coordinator.get_component<Sprite2dComponent>(entity);

            VkPipeline pipeline = RenderServer::getSingleton().blitGraphicsPipeline;

            VkBuffer vertexBuffers[] = {sprite.mesh->vertexBuffer};
            RenderServer::getSingleton().blit(
                    command_buffer,
                    pipeline,
                    sprite.mesh->getDescriptorSet(SwapChain::getSingleton().currentImage),
                    vertexBuffers,
                    sprite.mesh->indexBuffer,
                    sprite.mesh->indices_count);
        }
    }
}
