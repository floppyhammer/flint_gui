#include "model_render_system.h"

#include "../components/components.h"
#include "../coordinator.h"
#include "../../../render/swap_chain.h"
#include "../../../core/engine.h"
#include "../../../resources/mesh.h"
#include "../../../render/mvp_buffer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    void ModelRenderSystem::update() {
        auto coordinator = Coordinator::get_singleton();

        for (auto const &entity: entities) {
            auto &model = coordinator.get_component<ModelComponent>(entity);
            auto &transform = coordinator.get_component<Transform3dComponent>(entity);

            // Prepare MVP data.
            ModelViewProjection mvp{};

            // Determined by model transform.
            mvp.model = glm::translate(glm::mat4(1.0f), glm::vec3(transform.position.x, transform.position.y, transform.position.z));
            mvp.model = glm::scale(mvp.model, glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z));
            mvp.model = glm::rotate(mvp.model, (float) Engine::getSingleton().get_elapsed() * glm::radians(90.0f),
                                    glm::vec3(0.0f, 0.0f, 1.0f));

            mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, 1.0f));

            auto viewport_extent = SwapChain::getSingleton().swapChainExtent;

            // Set projection matrix. Determined by viewport.
            mvp.proj = glm::perspective(glm::radians(45.0f),
                                        (float) viewport_extent.width / (float) viewport_extent.height,
                                        0.1f,
                                        10.0f);

            // GLM was originally designed for OpenGL,
            // where the Y coordinate of the clip coordinates is inverted.
            mvp.proj[1][1] *= -1;

            model.push_constant.mvp = mvp.calculate_mvp();
        }
    }

    void ModelRenderSystem::draw(VkCommandBuffer command_buffer) {
        for (auto const &entity: entities) {
            auto coordinator = Coordinator::get_singleton();

            auto &model = coordinator.get_component<ModelComponent>(entity);

            VkPipeline pipeline = RenderServer::getSingleton().meshGraphicsPipeline;
            VkPipelineLayout pipeline_layout = RenderServer::getSingleton().meshPipelineLayout;

            // Upload the model matrix to the GPU via push constants.
            vkCmdPushConstants(command_buffer, pipeline_layout,
                               VK_SHADER_STAGE_VERTEX_BIT, 0,
                               sizeof(Mesh2dPushConstant), &model.push_constant.mvp);

            for (int i = 0; i < model.meshes.size(); i++) {
                const auto &mesh = model.meshes[i];
                const auto &desc_set = model.desc_sets[i];

                VkBuffer vertexBuffers[] = {mesh->vertexBuffer};
                RenderServer::getSingleton().draw_mesh(
                        command_buffer,
                        pipeline,
                        desc_set->getDescriptorSet(SwapChain::getSingleton().currentImage),
                        vertexBuffers,
                        mesh->indexBuffer,
                        mesh->indices_count);
            }
        }
    }
}