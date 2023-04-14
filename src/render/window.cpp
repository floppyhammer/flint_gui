#include "window.h"

#include <stdexcept>

#include "../common/utils.h"
#include "../servers/display_server.h"
#include "../servers/render_server.h"

using namespace Flint;

void Window::init(Vec2I size) {
    auto display_server = Flint::DisplayServer::get_singleton();

    // Create a GLFW window.
    glfw_window = display_server->create_window(size, "Flint");

    // Create a Vulkan window surface.
    create_surface();

    DisplayServer::get_singleton()->initialize_after_surface_creation(surface);
}

void Window::create_swapchain_related_resources(VkRenderPass render_pass, VkExtent2D swapchain_extent) {
    auto rs = RenderServer::get_singleton();
}

bool Window::should_close() const {
    return glfwWindowShouldClose(glfw_window);
}

VkExtent2D Window::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(glfw_window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                      std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

void Window::create_surface() {
    auto display_server = DisplayServer::get_singleton();

    if (glfwCreateWindowSurface(display_server->get_instance(), glfw_window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a Vulkan window surface!");
    }
}

VkSurfaceFormatKHR Window::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Window::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

void Window::cleanup() {
    auto instance = DisplayServer::get_singleton()->get_instance();

    cleanup_swapchain_related_resources();

    vkDestroySurfaceKHR(instance, surface, nullptr);

    glfwDestroyWindow(glfw_window);
}

void Window::cleanup_swapchain_related_resources() const {
}

bool Window::process() {
    //    if (should_close()) {
    //        // Wait on the host for the completion of outstanding queue operations for all queues on a given logical
    //        device. vkDeviceWaitIdle(DisplayServer::get_singleton()->get_device()); return false;
    //    }
    //
    //    //    // Collect input and window events.
    //    //    InputServer::get_singleton()->collect_events();
    //    //
    //    //    // Engine processing.
    //    //    Engine::get_singleton()->tick();
    //    //
    //    //    // Get frame time.
    //    //    auto dt = Engine::get_singleton()->get_delta();
    //
    //    // Acquire next image.
    //    // We should do this before updating the scenes as we need to modify different
    //    // buffers according to the current image index.
    //    uint32_t image_index;
    //    if (!swapchain->acquireSwapChainImage(image_index)) {
    //        Logger::error("Invalid swap chain image index!", "Swap Chain");
    //        return true;
    //    }
    //
    //    //    // Propagate input events.
    //    //    tree->input(InputServer::get_singleton()->input_queue);
    //    //
    //    //    // Update the scene tree.
    //    //    tree->update(dt);
    //
    //    // Record draw calls.
    //    record_commands(swapchain->commandBuffers, image_index);
    //
    //    // Submit commands for drawing.
    //    swapchain->flush(image_index);

    return true;
}
