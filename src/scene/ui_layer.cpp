#include "ui_layer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "render/swap_chain.h"
#include "resources/default_resource.h"
#include "scene/ui/node_ui.h"
#include "servers/vector_server.h"

namespace Flint {

UiLayer::UiLayer() {
    type = NodeType::UiLayer;

    update_mvp();

    mesh = DefaultResource::get_singleton()->new_default_mesh_2d();
}

void UiLayer::propagate_draw(VkCommandBuffer cmd_buffer) {
    // Record drawing commands.
    for (auto& child : children) {
        child->propagate_draw(cmd_buffer);
    }

    // Draw UI nodes.
    VectorServer::get_singleton()->submit();

    // TODO: let the scene tree manage UI layer drawing, so we can properly order UI and 2D/3D worlds.
    // Draw resulted UI texture.
    draw(cmd_buffer);
}

void UiLayer::update_mvp() {
    // Prepare MVP data. We use this matrix to convert a full-screen to the NodeGui's rect.
    ModelViewProjection mvp{};

    // The actual application order of these matrices is reverse.
    // 4.
    mvp.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    // 3.
    mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
    // 2.
    mvp.model = glm::scale(mvp.model, glm::vec3(1.0f, 1.0f, 1.0f));
    // 1.
    mvp.model = glm::scale(mvp.model, glm::vec3(2.0f, 2.0f, 1.0f));

    push_constant.mvp = mvp.model;
}

void UiLayer::draw(VkCommandBuffer cmd_buffer) {
    auto image_texture = VectorServer::get_singleton()->get_texture();
    mesh->surface->get_material()->set_texture(image_texture.get());

    auto rs = RenderServer::get_singleton();

    VkPipeline pipeline = rs->blit_pipeline;
    VkPipelineLayout pipeline_layout = rs->blit_pipeline_layout;

    // Upload the model matrix to the GPU via push constants.
    vkCmdPushConstants(
        cmd_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MvpPushConstant), &push_constant);

    // Unlike Sprite 2D, Texture Rect should not support custom mesh.
    rs->blit(cmd_buffer,
             pipeline,
             mesh->surface->get_material()->get_desc_set()->getDescriptorSet(SwapChain::get_singleton()->currentImage));
}

void UiLayer::when_window_size_changed(Vec2I new_size) {
    for (auto& child : children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi*>(child.get());
            cast_child->when_window_size_changed(new_size);
        }
    }
}

} // namespace Flint
