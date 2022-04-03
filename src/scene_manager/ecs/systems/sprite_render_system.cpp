#include "sprite_render_system.h"

#include "../components/components.h"
#include "../coordinator.h"
#include "../../../rendering/swap_chain.h"
#include "../../../rendering/mesh.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    void SpriteRenderSystem::update(float dt) {
        for (auto const &entity: entities) {
            auto coordinator = Coordinator::get_singleton();

            auto &sprite = coordinator.get_component<Sprite2D>(entity);
            auto &transform = coordinator.get_component<TransformGUI>(entity);

            if (sprite.uniform_buffers_memory.empty()) continue;

            // Default to swap chain image.
            auto extent = SwapChain::getSingleton().swapChainExtent;
            Vec2<uint32_t> viewport_extent = Vec2<uint32_t>(extent.width, extent.height);

            // Prepare UBO data. We use this matrix to convert a full-screen to the control's rect.
            UniformBufferObject ubo{};

            // The actual application order of these matrices is reverse.
            // 4.
            ubo.model = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(transform.rect_position.x / viewport_extent.x * 2.0f,
                                                 transform.rect_position.y / viewport_extent.y * 2.0f,
                                                 0.0f));
            // 3.
            ubo.model = glm::translate(ubo.model, glm::vec3(-1.0, -1.0, 0.0f));
            // 2.
            ubo.model = glm::scale(ubo.model, glm::vec3(transform.rect_scale.x, transform.rect_scale.y, 1.0f));
            // 1.
            ubo.model = glm::scale(ubo.model,
                                   glm::vec3(transform.rect_size.x / viewport_extent.x * 2.0f,
                                             transform.rect_size.y / viewport_extent.y * 2.0f,
                                             1.0f));

            // Copy the UBO data to the current uniform buffer.
            RS::getSingleton().copyDataToMemory(&ubo.model,
                                                sprite.uniform_buffers_memory[SwapChain::getSingleton().currentImage],
                                                sizeof(ubo.model));
        }
    }

    void SpriteRenderSystem::draw(VkCommandBuffer command_buffer) {
        for (auto const &entity: entities) {
            auto coordinator = Coordinator::get_singleton();

            auto &sprite = coordinator.get_component<Sprite2D>(entity);

            VkPipeline pipeline = RS::getSingleton().blitGraphicsPipeline;

            VkBuffer vertexBuffers[] = {sprite.mesh->vertexBuffer};
            RS::getSingleton().blit(
                    command_buffer,
                    pipeline,
                    sprite.mesh->getDescriptorSet(SwapChain::getSingleton().currentImage),
                    vertexBuffers,
                    sprite.mesh->indexBuffer,
                    sprite.mesh->indices_count);
        }
    }
}
