#include "app.h"

#include "rendering/device.h"
#include "rendering/swap_chain.h"
#include "rendering/rendering_server.h"
#include "core/engine.h"

#include <cstdint>
#include <memory>

void App::run() {
    // Initialize hardware.
    auto device = Device::getSingleton();

    // Initialize swap chain.
    auto swap_chain = SwapChain::getSingleton();

    // Initialize rendering server.
    // ---------------------------------------------------
    auto rs = RS::getSingleton();

    RS::getSingleton().createSwapChainRelatedResources(swap_chain.renderPass,
                                                       swap_chain.swapChainExtent);
    RS::getSingleton().p_swapChainExtent = swap_chain.swapChainExtent;
    RS::getSingleton().p_swapChainImages = &swap_chain.swapChainImages;
    // ---------------------------------------------------

    // Create command buffers for each swap chain images.
    createCommandBuffers();

    // Build scene tree. Use a block, so we don't increase ref counts for the nodes.
    // -----------------------------------
    {
        auto node = std::make_shared<Flint::Node>();
        auto node_3d = std::make_shared<Flint::Node3D>();
        auto mesh_instance_0 = std::make_shared<Flint::MeshInstance3D>();
        auto mesh_instance_1 = std::make_shared<Flint::MeshInstance3D>();

        node->add_child(node_3d);
        node_3d->add_child(mesh_instance_0);
        node_3d->add_child(mesh_instance_1);
        mesh_instance_0->position.x = 1;
        mesh_instance_1->position.x = -1;
        tree.set_root(node);
    }
    // -----------------------------------

    mainLoop();

    // Release node resources.
    tree.set_root(nullptr);

    swap_chain.cleanup();

    rs.cleanup();

    device.cleanup();
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(Device::getSingleton().window)) {
        glfwPollEvents();
        drawFrame();
    }

    // Wait on the host for the completion of outstanding queue operations for all queues on a given logical device.
    vkDeviceWaitIdle(Device::getSingleton().device);
}

void App::createCommandBuffers() {
    commandBuffers.resize(SwapChain::getSingleton().swapChainFramebuffers.size());

    // Allocate command buffers.
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = RS::getSingleton().commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if (vkAllocateCommandBuffers(Device::getSingleton().device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

void App::recordCommands(uint32_t imageIndex) {
    // Reset command buffer.
    vkResetCommandBuffer(commandBuffers[imageIndex], 0);

    // Begin recording.
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Begin render pass.
    // ----------------------------------------------
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
    // ----------------------------------------------

    // Record commands from the scene tree.
    // ---------------------------------------------
    RS::getSingleton().p_commandBuffer = commandBuffers[imageIndex];
    RS::getSingleton().p_currentImage = imageIndex;
    tree.record_commands();
    // ---------------------------------------------

    // End render pass.
    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    // End recording.
    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
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
    recordCommands(imageIndex);

    // Submit commands for drawing.
    SwapChain::getSingleton().flush(commandBuffers, imageIndex);
}
