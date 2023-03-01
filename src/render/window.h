#ifndef FLINT_WINDOW_H
#define FLINT_WINDOW_H

#include <iostream>
#include <optional>
#include <vector>

#include "../common/geometry.h"
#include "../servers/display_server.h"

using Pathfinder::Vec2I;
using namespace Flint;

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

/// How many frames should be processed concurrently.
const int MAX_FRAMES_IN_FLIGHT = 2;

class SwapChain;

class Window {
public:
    Window() = default;

    void init(Vec2I size);

    bool process();

    bool should_close() const;

    GLFWwindow *glfw_window{};

    /// Native platform surface or window objects are abstracted by surface objects.
    VkSurfaceKHR surface{};

    VkQueue presentQueue{};

    /// Flag indicating the window size has changed.
    bool framebufferResized = false;

    int32_t framebuffer_width{};
    int32_t framebuffer_height{};

    static void framebufferResizeCallback(GLFWwindow *glfw_window, int width, int height) {
        // We have attached a Window pointer to this window previously. Now we retrieve it.
        auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        window->framebufferResized = true;
        window->framebuffer_width = width;
        window->framebuffer_height = height;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    void cleanup();

    void create_swapchain_related_resources(VkRenderPass render_pass, VkExtent2D swapchain_extent);

    void cleanup_swapchain_related_resources() const;

private:
    void create_surface();
};

#endif // FLINT_WINDOW_H
