#include "world.h"

#include "../common/logger.h"
#include "../render/swap_chain.h"
#include "2d/camera2d.h"
#include "3d/camera3d.h"
#include "resources/default_resource.h"

namespace Flint {

World::World(bool _is_2d) {
    type = NodeType::World;

    is_2d = _is_2d;

    mesh = DefaultResource::get_singleton()->new_default_mesh_2d();

    update_mvp();
}

void World::propagate_draw(VkCommandBuffer cmd_buffer) {
    RenderTarget* render_target;
    ColorF clear_color;
    // Handle each active camera in this world, and redraw the subtree for each camera.
    if (is_2d) {
        for (auto& cam : camera2ds) {
            current_camera2d = cam;
            render_target = current_camera2d->render_target.get();
            clear_color = current_camera2d->clear_color.to_f32();

            draw_subtree(render_target, clear_color, current_camera2d->get_texture().get());

            draw(cmd_buffer);
        }
    } else {
        for (auto& cam : camera3ds) {
            current_camera3d = cam;
            render_target = current_camera3d->render_target.get();
            clear_color = current_camera3d->clear_color.to_f32();

            draw_subtree(render_target, clear_color, current_camera3d->get_texture().get());

            draw(cmd_buffer);
        }
    }
}

void World::draw_subtree(RenderTarget* render_target, ColorF clear_color, ImageTexture* image_texture) {
    auto cmd_buffer = RenderServer::getSingleton()->beginSingleTimeCommands();

    // Begin render pass.
    // It seems not feasible to wrap begin info into rendering Viewport.
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = render_target->renderPass;
    renderPassInfo.framebuffer = render_target->framebuffer; // Set target framebuffer.
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent =
        VkExtent2D{(uint32_t)render_target->get_extent().x, (uint32_t)render_target->get_extent().y};

    // Clear color.
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{clear_color.r, clear_color.g, clear_color.b, clear_color.a}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Start recursive calling to draw all node under this sub-viewport.
    for (auto &child : children) {
        child->propagate_draw(cmd_buffer);
    }

    // End render pass.
    vkCmdEndRenderPass(cmd_buffer);

    RenderServer::getSingleton()->endSingleTimeCommands(cmd_buffer);

    image_texture->layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void World::add_camera2d(Camera2d* new_camera) {
    if (!is_2d) {
        Logger::error("Failed to add a Camera2d to a 3D World!", "World");
        return;
    }

    camera2ds.push_back(new_camera);
}

void World::add_camera3d(Camera3d* new_camera) {
    if (is_2d) {
        Logger::error("Failed to add a Camera3d to a 2D World!", "World");
        return;
    }

    current_camera3d = new_camera;
    camera3ds.push_back(new_camera);
}

void World::update_mvp() {
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

void World::draw(VkCommandBuffer cmd_buffer) {
    std::shared_ptr<ImageTexture> image_texture;

    if (is_2d) {
        image_texture = current_camera2d->get_texture();
    } else {
        image_texture = current_camera3d->get_texture();
    }

    mesh->surface->get_material()->set_texture(image_texture.get());

    VkPipeline pipeline = RenderServer::getSingleton()->blitGraphicsPipeline;
    VkPipelineLayout pipeline_layout = RenderServer::getSingleton()->blitPipelineLayout;

    // Upload the model matrix to the GPU via push constants.
    vkCmdPushConstants(
        cmd_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MvpPushConstant), &push_constant);

    RenderServer::getSingleton()->blit(
        cmd_buffer,
        pipeline,
        mesh->surface->get_material()->get_desc_set()->getDescriptorSet(SwapChain::getSingleton()->currentImage));
}

} // namespace Flint
