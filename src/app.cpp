#include "app.h"

#include <cstdint>
#include <memory>

#include "common/load_file.h"
#include "common/obj_importer.h"
#include "render/platform.h"
#include "render/render_server.h"
#include "render/swap_chain.h"
#include "resources/default_resource.h"
#include "resources/image_texture.h"
#include "resources/mesh.h"
#include "resources/resource_manager.h"
#include "servers/core_server.h"
#include "servers/display_server.h"
#include "servers/input_server.h"
#include "servers/vector_server.h"

using namespace Flint;

void App::record_commands(std::vector<VkCommandBuffer> &command_buffers, uint32_t image_index) const {
    // Reset the current command buffer.
    vkResetCommandBuffer(command_buffers[image_index], 0);

    // Begin recording commands.
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(command_buffers[image_index], &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Begin main render pass. We bind the target framebuffer here.
    // We should do this only once for the main render pass due to the clear operation.
    {
        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = SwapChain::get_singleton()->renderPass;
        render_pass_info.framebuffer =
            SwapChain::get_singleton()->swapChainFramebuffers[image_index]; // Set target framebuffer.
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent =
            SwapChain::get_singleton()->swapChainExtent; // Has to be larger than the area we're going to draw.

        // Clear color.
        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
        clear_values[1].depthStencil = {1.0f, 0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(command_buffers[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record drawing commands from the scene manager.
    tree->draw(command_buffers[image_index]);

    // End main render pass.
    vkCmdEndRenderPass(command_buffers[image_index]);

    // End recording commands.
    if (vkEndCommandBuffer(command_buffers[image_index]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

App::App(int32_t window_width, int32_t window_height) {
    // Set logger level.
    Logger::set_level(Logger::VERBOSE);

    // 1. Initialize hardware.
    auto window = Window::get_singleton();
    window->init(window_width, window_height);

    auto display_server = DisplayServer::get_singleton();

    // 2. Initialize render server.
    auto render_server = RenderServer::get_singleton();

    // 3. Initialize swap chain.
    auto swap_chain = SwapChain::get_singleton();

    // 4. Initialize input server.
    auto input_server = InputServer::get_singleton();
    input_server->init(window->glfw_window);

    // 5. Initialize vector server.
    std::shared_ptr<Pathfinder::Driver> driver = std::make_shared<Pathfinder::DriverVk>(
        window->device, window->physicalDevice, window->graphicsQueue, render_server->command_pool);
    auto vector_server = VectorServer::get_singleton();
    vector_server->init(driver, window_width, window_height);

    tree = std::make_unique<Flint::SceneTree>();
}

void App::main_loop() {
    while (!Window::get_singleton()->should_close()) {
        // Collect input and window events.
        InputServer::get_singleton()->collect_events();

        // Engine processing.
        CoreServer::get_singleton()->tick();

        // Get frame time.
        auto dt = CoreServer::get_singleton()->get_delta();

        // Acquire next image.
        // We should do this before updating the scenes as we need to modify different
        // buffers according to the current image index.
        uint32_t image_index;
        if (!SwapChain::get_singleton()->acquireSwapChainImage(image_index)) {
            Logger::error("Invalid swap chain image index!", "Swap Chain");
            return;
        }

        // Propagate input events.
        tree->input(InputServer::get_singleton()->input_queue);

        // Update the scene tree.
        tree->update(dt);

        // Record draw calls.
        record_commands(SwapChain::get_singleton()->commandBuffers, image_index);

        // Submit commands for drawing.
        SwapChain::get_singleton()->flush(image_index);
    }

    // Wait on the host for the completion of outstanding queue operations for all queues on a given logical device.
    vkDeviceWaitIdle(Window::get_singleton()->device);
}

void App::cleanup() {
    // Clean up the scene tree.
    tree.reset();

    VectorServer::get_singleton()->cleanup();
    Logger::verbose("Cleaned up VectorServer.", "App");

    DefaultResource::get_singleton()->cleanup();
    Logger::verbose("Cleaned up DefaultResource.", "App");

    SwapChain::get_singleton()->cleanup();
    Logger::verbose("Cleaned up SwapChain.", "App");

    RenderServer::get_singleton()->cleanup();
    Logger::verbose("Cleaned up RenderServer.", "App");

    Window::get_singleton()->cleanup();
    Logger::verbose("Cleaned up Window.", "App");
}
