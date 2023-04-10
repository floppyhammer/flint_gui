#include "sprite2d.h"

#include "../../common/logger.h"
#include "../../render/mvp.h"
#include "../../render/swap_chain.h"
#include "../world.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint::Scene {

Sprite2d::Sprite2d() {
    type = NodeType::Sprite2d;
}

void Sprite2d::set_texture(std::shared_ptr<ImageTexture> p_texture) {
    if (!p_texture) return;

    texture = p_texture;

    mesh->surface->get_material()->set_texture(p_texture.get());
}

std::shared_ptr<ImageTexture> Sprite2d::get_texture() const {
    return texture;
}

void Sprite2d::set_mesh(const std::shared_ptr<Mesh2d> &p_mesh) {
    mesh = p_mesh;
}

void Sprite2d::set_material(const std::shared_ptr<Material2d> &p_material) {
    mesh->surface->set_material(p_material);
}

void Sprite2d::update(double delta) {
    update_mvp();
}

void Sprite2d::update_mvp() {
    if (mesh == nullptr || mesh->surface == nullptr) {
        return;
    }
//
//    Vec2I view_size = get_view_size();
//
//    auto scaled_size = mesh->surface->get_material()->get_texture()->get_size().to_f32() * scale;
//
//    auto global_position = get_global_position();
//
//    // Prepare MVP data. We use this matrix to convert a full-screen to the control's rect.
//    ModelViewProjection mvp{};
//
//    // The actual application order of these matrices is reverse.
//    // 3.
//    mvp.model = glm::translate(glm::mat4(1.0f),
//                               glm::vec3((global_position.x - scaled_size.x * 0.5) / view_size.x * 2.0f,
//                                         (global_position.y - scaled_size.y * 0.5) / view_size.y * 2.0f,
//                                         0.0f));
//    // 2.
//    mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
//    // 1.
//    mvp.model =
//        glm::scale(mvp.model, glm::vec3(scaled_size.x / view_size.x * 2.0f, scaled_size.y / view_size.y * 2.0f, 1.0f));
//
//    push_constant.mvp = mvp.model;
}

void Sprite2d::draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) {
    if (mesh == nullptr) {
        Utils::Logger::warn("No valid mesh set for Sprite 2D!");
        return;
    }
//
//    Node *viewport_node = get_viewport();
//
//    VkPipeline pipeline = RenderServer::get_singleton()->blitGraphicsPipeline;
//    VkPipelineLayout pipeline_layout = RenderServer::get_singleton()->blitPipelineLayout;
//
//    if (viewport_node) {
//        auto viewport = dynamic_cast<SubViewport *>(viewport_node);
//        pipeline = viewport->render_target->blitGraphicsPipeline;
//    }
//
//    // Upload the model matrix to the GPU via push constants.
//    vkCmdPushConstants(
//        p_command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MvpPushConstant), &push_constant);
//
//    VkBuffer vertexBuffers[] = {mesh->surface->get_vertex_buffer()};
//
//    // Unlike Texture Rect, Sprite 2D should support custom mesh.
//    RenderServer::get_singleton()->draw_mesh_2d(
//        p_command_buffer,
//        pipeline,
//        mesh->surface->get_material()->get_desc_set()->getDescriptorSet(SwapChain::get_singleton()->currentImage),
//        vertexBuffers,
//        mesh->surface->get_index_buffer(),
//        mesh->surface->get_index_count());
//
//    Node2d::draw(p_command_buffer);
}

} // namespace Flint
