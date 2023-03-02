#include "window_proxy.h"

#include "../servers/vector_server.h"

namespace Flint {

WindowProxy::WindowProxy(Vec2I _size, bool _dummy) {
    type = NodeType::Window;

    size = _size;

    dummy = _dummy;

    window = std::make_shared<Window>();
    window->init(size);
    DisplayServer::get_singleton()->initialize_after_surface_creation(window->surface);
    DisplayServer::get_singleton()->create_graphics_queues(window->surface, window->presentQueue);

    swapchain = std::make_shared<Flint::SwapChain>(window.get());
}

Vec2I WindowProxy::get_size() const {
    return size;
}

void WindowProxy::record_commands(std::vector<VkCommandBuffer> &command_buffers, uint32_t image_index) const {
    auto cmd_buffer = command_buffers[image_index];

    // Reset the current command buffer.
    vkResetCommandBuffer(cmd_buffer, 0);

    // Begin recording commands.
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(cmd_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Begin main render pass. We bind the target framebuffer here.
    // We should do this only once for the main render pass due to the clear operation.
    {
        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = swapchain->renderPass;
        render_pass_info.framebuffer = swapchain->swapChainFramebuffers[image_index]; // Set target framebuffer.
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent =
            swapchain->swapChainExtent; // Has to be larger than the area we're going to draw.

        // Clear color.
        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
        clear_values[1].depthStencil = {1.0f, 0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(cmd_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Set viewport and scissor dynamically.
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = size.x;
    viewport.height = size.y;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent.width = size.x;
    scissor.extent.height = size.y;
    vkCmdSetScissor(cmd_buffer, 0, 1, &scissor);

    // Record drawing commands from the scene manager.
    for (auto &child : children) {
        child->propagate_draw(swapchain->renderPass, cmd_buffer);
    }

    // End main render pass.
    vkCmdEndRenderPass(cmd_buffer);

    // End recording commands.
    if (vkEndCommandBuffer(cmd_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

void WindowProxy::propagate_input(InputEvent &event) {
    // Filter events not belonging to this window.
    if (event.window != window->glfw_window) {
        return;
    }

    auto it = children.rbegin();
    while (it != children.rend()) {
        (*it)->propagate_input(event);
        it++;
    }

    input(event);
}

void WindowProxy::propagate_draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) {
    // Acquire next image.
    // We should do this before updating the scenes as we need to modify different
    // buffers according to the current image index.
    uint32_t image_index;
    if (!swapchain->acquireSwapChainImage(image_index)) {
        Logger::error("Invalid swap chain image index!", "Swap Chain");
        return;
    }

    record_commands(swapchain->commandBuffers, image_index);

    // Submit commands for drawing.
    swapchain->flush(image_index);
}

} // namespace Flint
