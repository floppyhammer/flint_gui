#include "sub_viewport.h"

#include <array>

#include "../common/logger.h"
#include "../render/swap_chain.h"

namespace Flint {

SubViewport::SubViewport() {
    type = NodeType::SubViewport;

    render_target = std::make_shared<RenderTarget>();
}

void SubViewport::propagate_draw(VkCommandBuffer cmd_buffer) {
    // Begin render pass.
    {
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
    }

    // Start recursive calling to draw all node under this sub-viewport.
    Node::propagate_draw(cmd_buffer);

    // End render pass.
    vkCmdEndRenderPass(cmd_buffer);
}

std::shared_ptr<ImageTexture> SubViewport::get_texture() const {
    return render_target->texture;
}

Vec2I SubViewport::get_extent() const {
    return render_target->get_extent();
}

void SubViewport::set_camera2d(Camera2d* _camera2d) {
    if (!is_2d) {
        Logger::error("Failed to set a Camera2d to a 3D SubViewport!", "SubViewport");
        return;
    }

    camera2d = _camera2d;
}

void SubViewport::set_camera3d(Camera3d* _camera3d) {
    if (is_2d) {
        Logger::error("Failed to set a Camera3d to a 2D SubViewport!", "SubViewport");
        return;
    }

    camera3d = _camera3d;
}

void SubViewport::get_view_uniform() const {
}

} // namespace Flint
