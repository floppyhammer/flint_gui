#include "model_render_system.h"

#include "../components/components.h"
#include "../coordinator.h"
#include "../../../rendering/swap_chain.h"
#include "../../../core/engine.h"
#include "../../../rendering/mesh.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    void ModelRenderSystem::update(float dt) {
        for (auto const &entity: entities) {
            auto coordinator = Coordinator::get_singleton();

            auto &model = coordinator.get_component<Model>(entity);
            auto &transform = coordinator.get_component<Transform3D>(entity);

            if (model.uniform_buffers_memory.empty()) continue;

            // Prepare UBO data.
            UniformBufferObject ubo{};

            // Determined by model transform.
            ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(transform.position.x, transform.position.y, transform.position.z));
            ubo.model = glm::scale(ubo.model, glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z));
            ubo.model = glm::rotate(ubo.model, (float) Engine::getSingleton().get_elapsed() * glm::radians(90.0f),
                                    glm::vec3(0.0f, 0.0f, 1.0f));

            ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, 1.0f));

            auto viewport_extent = SwapChain::getSingleton().swapChainExtent;

            // Set projection matrix. Determined by viewport.
            ubo.proj = glm::perspective(glm::radians(45.0f),
                                        (float) viewport_extent.width / (float) viewport_extent.height,
                                        0.1f,
                                        10.0f);

            // GLM was originally designed for OpenGL,
            // where the Y coordinate of the clip coordinates is inverted.
            ubo.proj[1][1] *= -1;

            // Copy the UBO data to the current uniform buffer.
            RS::getSingleton().copyDataToMemory(&ubo,
                                                model.uniform_buffers_memory[SwapChain::getSingleton().currentImage],
                                                sizeof(ubo));
        }
    }

    void ModelRenderSystem::draw(VkCommandBuffer command_buffer) {
        for (auto const &entity: entities) {
            auto coordinator = Coordinator::get_singleton();

            auto &model = coordinator.get_component<Model>(entity);

            VkPipeline pipeline = RS::getSingleton().meshGraphicsPipeline;

            for (const auto &mesh: model.meshes) {
                VkBuffer vertexBuffers[] = {mesh->vertexBuffer};
                RS::getSingleton().draw_mesh(
                        command_buffer,
                        pipeline,
                        mesh->getDescriptorSet(SwapChain::getSingleton().currentImage),
                        vertexBuffers,
                        mesh->indexBuffer,
                        mesh->indices_count);
            }
        }
    }
}
