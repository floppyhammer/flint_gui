#include "app.h"

#include "rendering/device.h"
#include "rendering/swap_chain.h"
#include "rendering/rendering_server.h"
#include "rendering/texture.h"
#include "core/engine.h"
#include "scenes/gui/sub_viewport_container.h"
#include "scenes/gui/texture_rect.h"
#include "scenes/sub_viewport.h"

#include <cstdint>
#include <memory>

void App::run() {
    Flint::Logger::set_level(Flint::Logger::WARN);

    // Initialization.
    // ---------------------------------------------------
    // 1. Initialize hardware.
    auto device = Device::getSingleton();

    // 2. Initialize rendering server.
    auto rs = RS::getSingleton();

    // 3. Initialize swap chain.
    auto swap_chain = SwapChain::getSingleton();
    // ---------------------------------------------------

    // Build scene tree. Use a block, so we don't increase ref counts for the nodes.
    {
        auto node = std::make_shared<Flint::Node>();
        auto node_3d = std::make_shared<Flint::Node3D>();
        auto mesh_instance_0 = std::make_shared<Flint::MeshInstance3D>();
        auto mesh_instance_1 = std::make_shared<Flint::MeshInstance3D>();
        auto sub_viewport_c = std::make_shared<Flint::SubViewportContainer>();
        auto sub_viewport = std::make_shared<Flint::SubViewport>();
        auto texture_rect = std::make_shared<Flint::TextureRect>();
        texture_rect->set_texture(Texture::from_file("../res/texture.jpg"));

        node->add_child(texture_rect);
        node->add_child(mesh_instance_0);
        node->add_child(sub_viewport_c);
        sub_viewport_c->add_child(sub_viewport);
        sub_viewport_c->set_viewport(sub_viewport);
        sub_viewport->add_child(node_3d);
        node_3d->add_child(mesh_instance_1);
        texture_rect->set_rect_scale(0.3, 0.3);
        texture_rect->set_rect_position(400, 0);
        mesh_instance_0->position.x = 1;
        //mesh_instance_0->scale.x = mesh_instance_0->scale.y = mesh_instance_0->scale.z = 0.02;
        mesh_instance_1->position.x = -1;
        //mesh_instance_1->scale.x = mesh_instance_1->scale.y = mesh_instance_1->scale.z = 0.02;
        tree.set_root(node);
    }

    mainLoop();

    // Cleanup.
    {
        // Release node resources.
        tree.set_root(nullptr);

        swap_chain.cleanup();

        rs.cleanup();

        device.cleanup();
    }
}

void App::recordCommands(std::vector<VkCommandBuffer> &commandBuffers, uint32_t imageIndex) const {
    // Reset command buffer.
    vkResetCommandBuffer(commandBuffers[imageIndex], 0);

    // Begin recording.
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Begin render pass. We can only do this for the swap chain render pass once due to the clear operation.
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = SwapChain::getSingleton().renderPass;
        renderPassInfo.framebuffer = SwapChain::getSingleton().swapChainFramebuffers[imageIndex]; // Set target framebuffer.
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = SwapChain::getSingleton().swapChainExtent;

        // Clear color.
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex],
                             &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record commands from the scene tree.
    tree.draw(commandBuffers[imageIndex]);

    // End render pass.
    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    // End recording.
    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(Device::getSingleton().window)) {
        glfwPollEvents();
        drawFrame();
    }

    // Wait on the host for the completion of outstanding queue operations for all queues on a given logical device.
    vkDeviceWaitIdle(Device::getSingleton().device);
}

void App::drawFrame() {
    // Engine processing.
    Flint::Engine::getSingleton().tick();

    // Acquire next image.
    uint32_t imageIndex;
    if (!SwapChain::getSingleton().acquireSwapChainImage(imageIndex)) return;

    // Update the scene tree.
    tree.update(Flint::Engine::getSingleton().get_delta());

    // Record draw calls.
    recordCommands(SwapChain::getSingleton().commandBuffers, imageIndex);

    // Submit commands for drawing.
    SwapChain::getSingleton().flush(imageIndex);

    //Flint::Logger::verbose("---------------- FRAME ----------------", "Main Loop");
}
