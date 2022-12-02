#include "sub_viewport_container.h"

#include <array>
#include <utility>

#include "../../render/swap_chain.h"
#include "../../resources/default_resource.h"
#include "../sub_viewport.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
SubViewportContainer::SubViewportContainer() {
    type = NodeType::SubViewportContainer;

    mesh = DefaultResource::get_singleton()->new_default_mesh_2d();
}

void SubViewportContainer::set_viewport(std::shared_ptr<SubViewport> p_viewport) {
    sub_viewport = std::move(p_viewport);

    mesh->surface->get_material()->set_texture(sub_viewport->get_texture().get());
}

void SubViewportContainer::update(double dt) {
    update_mvp();
}

void SubViewportContainer::propagate_draw(VkCommandBuffer p_command_buffer) {
    // Don't call Node::_draw(), so we can break the recursive calling
    // to call the sub-viewport draw function below specifically.
    // Also, we can't interrupt our previous render pass.

    auto sub_viewport_command_buffer = RenderServer::getSingleton()->beginSingleTimeCommands();

    // Start sub-viewport render pass.
    if (sub_viewport != nullptr) {
        sub_viewport->propagate_draw(sub_viewport_command_buffer);
    }

    RenderServer::getSingleton()->endSingleTimeCommands(sub_viewport_command_buffer);

    // Now draw the rendered sub-viewport texture.
    draw(p_command_buffer);
}

void SubViewportContainer::draw(VkCommandBuffer cmd_buffer) {
    Node *viewport_node = get_viewport();

    VkPipeline pipeline = RenderServer::getSingleton()->blitGraphicsPipeline;
    VkPipelineLayout pipeline_layout = RenderServer::getSingleton()->blitPipelineLayout;

    if (viewport_node) {
        auto viewport = dynamic_cast<SubViewport *>(viewport_node);
        pipeline = viewport->render_target->blitGraphicsPipeline;
    }

    // Upload the model matrix to the GPU via push constants.
    vkCmdPushConstants(
        cmd_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MvpPushConstant), &push_constant);

    RenderServer::getSingleton()->blit(
        cmd_buffer,
        pipeline,
        mesh->surface->get_material()->get_desc_set()->getDescriptorSet(SwapChain::getSingleton()->currentImage));
}

void SubViewportContainer::update_mvp() {
    Vec2I viewport_extent = get_viewport_size();

    // Prepare MVP data. We use this matrix to convert a full-screen to the NodeGui's rect.
    ModelViewProjection mvp{};

    // The actual application order of these matrices is reverse.
    // 4.
    mvp.model = glm::translate(
        glm::mat4(1.0f), glm::vec3(position.x / viewport_extent.x * 2.0f, position.y / viewport_extent.y * 2.0f, 0.0f));
    // 3.
    mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
    // 2.
    mvp.model = glm::scale(mvp.model, glm::vec3(scale.x, scale.y, 1.0f));
    // 1.
    mvp.model =
        glm::scale(mvp.model, glm::vec3(size.x / viewport_extent.x * 2.0f, size.y / viewport_extent.y * 2.0f, 1.0f));

    push_constant.mvp = mvp.model;
}

} // namespace Flint
