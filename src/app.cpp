#include "app.h"

#include "render/platform.h"
#include "render/swap_chain.h"
#include "render/render_server.h"
#include "resources/image_texture.h"
#include "resources/mesh.h"
#include "resources/resource_manager.h"
#include "core/engine.h"
#include "servers/input_server.h"
#include "servers/vector_server.h"
#include "io/obj_importer.h"
#include "common/io.h"

#include "scene/ecs/components/components.h"

#include <cstdint>
#include <memory>

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
        render_pass_info.renderPass = SwapChain::getSingleton()->renderPass;
        render_pass_info.framebuffer = SwapChain::getSingleton()->swapChainFramebuffers[image_index]; // Set target framebuffer.
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = SwapChain::getSingleton()->swapChainExtent; // Has to be larger than the area we're going to draw.

        // Clear color.
        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
        clear_values[1].depthStencil = {1.0f, 0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(command_buffers[image_index],
                             &render_pass_info,
                             VK_SUBPASS_CONTENTS_INLINE);
    }

    VectorServer::get_singleton()->clear_scene();

    // Record commands from the scene manager.
    {
        tree->draw(command_buffers[image_index]);

        world->draw(command_buffers[image_index]);
    }

    // FIXME: When nothing is drawn, the dest image layout will not be set to SHADER_READ_ONLY.
    VectorServer::get_singleton()->submit();

    // End main render pass.
    vkCmdEndRenderPass(command_buffers[image_index]);

    // End recording commands.
    if (vkEndCommandBuffer(command_buffers[image_index]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

App::App(uint32_t window_width, uint32_t window_height) {
    // Set logger level.
    Logger::set_level(Logger::VERBOSE);

    // 1. Initialize hardware.
    auto platform = Platform::getSingleton();
    platform->init(window_width, window_height);

    // 2. Initialize render server.
    auto render_server = RenderServer::getSingleton();

    // 3. Initialize swap chain.
    auto swap_chain = SwapChain::getSingleton();

    // 4. Initialize input server.
    auto input_server = InputServer::get_singleton();
    input_server->init(platform->window);

    // 5. Initialize vector server.
    std::shared_ptr<Pathfinder::Driver> driver = std::make_shared<Pathfinder::DriverVk>(
            platform->device,
            platform->physicalDevice,
            platform->graphicsQueue,
            render_server->commandPool);
    auto vector_server = VectorServer::get_singleton();
    vector_server->init(driver,
                        window_width,
                        window_height);

    tree = std::make_unique<Flint::SceneTree>();
    world = std::make_unique<World>();
}

void App::main_loop() {
    while (!glfwWindowShouldClose(Platform::getSingleton()->window)) {
        // Collect input and window events.
        InputServer::get_singleton()->collect_events();

        // Engine processing.
        Engine::get_singleton()->tick();

        // Get frame time.
        auto dt = Engine::get_singleton()->get_delta();

        // Acquire next image.
        // We should do this before updating the scenes as we need to modify different
        // buffers according to the current image index.
        uint32_t image_index;
        if (!SwapChain::getSingleton()->acquireSwapChainImage(image_index)) {
            Logger::error("Invalid swap chain image index!", "Swap Chain");
            return;
        }

        // Update the scene.
        {
            tree->input(InputServer::get_singleton()->input_queue);

            // Node scene manager.
            tree->update(dt);

            // ECS scene manager.
            world->update(dt);
        }

        // Record draw calls.
        record_commands(SwapChain::getSingleton()->commandBuffers, image_index);

        // Submit commands for drawing.
        SwapChain::getSingleton()->flush(image_index);
    }

    // Wait on the host for the completion of outstanding queue operations for all queues on a given logical device.
    vkDeviceWaitIdle(Platform::getSingleton()->device);
}

void App::cleanup() {
    // Clean up the scene.
    tree.reset();
    world.reset();

    VectorServer::get_singleton()->cleanup();
    Logger::verbose("Cleaned up VectorServer.", "App");

    DefaultResource::get_singleton()->cleanup();
    Logger::verbose("Cleaned up DefaultResource.", "App");

    SwapChain::getSingleton()->cleanup();
    Logger::verbose("Cleaned up SwapChain.", "App");

    RenderServer::getSingleton()->cleanup();
    Logger::verbose("Cleaned up RenderServer.", "App");

    Platform::getSingleton()->cleanup();
    Logger::verbose("Cleaned up Platform.", "App");
}
