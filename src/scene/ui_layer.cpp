#include "ui_layer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "render/swap_chain.h"
#include "resources/default_resource.h"
#include "scene/ui/node_ui.h"
#include "scene/window_proxy.h"
#include "servers/vector_server.h"

namespace Flint {

UiLayer::UiLayer() {
    type = NodeType::UiLayer;

    update_mvp();

    surface = DefaultResource::get_singleton()->new_default_surface_2d();
}

void UiLayer::propagate_draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) {
    if (!get_window()) {
        return;
    }

    if (!texture) {
        texture = ImageTexture::from_empty(get_window()->get_size(), VK_FORMAT_R8G8B8A8_UNORM);
    }

    auto vs = VectorServer::get_singleton();

    // Save the scene which belongs to another UiLayer;
    auto old_scene = vs->get_canvas()->take_scene();

    // Record drawing commands.
    for (auto& child : children) {
        child->propagate_draw(render_pass, cmd_buffer);
    }

    // Set the dst texture just before drawing, in case we have other child UiLayers.
    vs->set_dst_texture(texture);

    vs->submit_and_clear();

    // Restore the old scene;
    vs->get_canvas()->set_scene(old_scene);

    // TODO: let the scene tree manage UI layer drawing, so we can properly order UI and 2D/3D worlds.
    // Draw resulted UI texture.
    draw(render_pass, cmd_buffer);
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

void UiLayer::draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) {
    surface->get_material()->set_texture(texture.get());

    auto rs = RenderServer::get_singleton();

    VkPipeline pipeline = rs->get_pipeline(render_pass, "mesh2d");
    VkPipelineLayout pipeline_layout = rs->blit_pipeline_layout;

    // Upload the model matrix to the GPU via push constants.
    vkCmdPushConstants(
        cmd_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MvpPushConstant), &push_constant);

    // Unlike Sprite 2D, Texture Rect should not support custom mesh.
    rs->blit(cmd_buffer, pipeline, surface->get_material()->get_desc_set()->getDescriptorSet(get_current_image()));
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
