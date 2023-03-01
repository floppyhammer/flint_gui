#include "display_server.h"

namespace Flint {

DisplayServer::DisplayServer() {
    // Initializes GLFW.
    glfwInit();
}

DisplayServer::~DisplayServer() {
    glfwTerminate();
}

GLFWwindow* DisplayServer::create_window(Vec2I size, std::string title) {
    // To not create an OpenGL context (as we're using Vulkan).
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Enable window resizing. Resizing requires swap chain recreation.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Hide window upon creation as we need to center the window before showing it.
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    // Get monitor position (used to correctly center the window in a multi-monitor scenario).
    int monitors_count;
    GLFWmonitor** monitors = glfwGetMonitors(&monitors_count);

    const GLFWvidmode* video_mode = glfwGetVideoMode(monitors[0]);

    int monitor_x, monitor_y;
    glfwGetMonitorPos(monitors[0], &monitor_x, &monitor_y);

    // Get DPI scale.
    float dpi_scale_x, dpi_scale_y;
    glfwGetMonitorContentScale(monitors[0], &dpi_scale_x, &dpi_scale_y);

    GLFWwindow* glfw_window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);

    // Center window.
    glfwSetWindowPos(
        glfw_window, monitor_x + (video_mode->width - size.x) / 2, monitor_y + (video_mode->height - size.y) / 2);

    // Show window.
    glfwShowWindow(glfw_window);

    // Assign this to window user, so we can fetch it when the window size changes.
    glfwSetWindowUserPointer(glfw_window, this);
    //        glfwSetFramebufferSizeCallback(glfw_window, framebufferResizeCallback);

    return glfw_window;
}

} // namespace Flint
