#ifndef FLINT_DISPLAY_SERVER_H
#define FLINT_DISPLAY_SERVER_H

#include "../common/geometry.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <optional>
#include <vector>

using Pathfinder::Vec2;
using Pathfinder::Vec2F;

namespace Flint {

/// List of required validation layers.
const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

/// List of required device extensions.
const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class DisplayServer {
public:
    static DisplayServer *get_singleton() {
        static DisplayServer singleton;
        return &singleton;
    }

    DisplayServer();

    void cleanup();

    GLFWwindow *create_window(Vec2I size, const std::string &title);

    VkInstance get_instance() const {
        return instance;
    }

    VkDevice get_device() const {
        return device;
    }

    VkQueue get_graphics_queue() const {
        return graphicsQueue;
    }

    static const bool enableValidationLayers = true;

    void create_graphics_queues(VkSurfaceKHR surface, VkQueue &graphicsQueue);

    void create_present_queues(VkSurfaceKHR surface, VkQueue &presentQueue);

    VkCommandPool command_pool;

    [[nodiscard]] uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const;

    /// The graphics card that we'll end up selecting will be stored in a VkPhysicalDevice handle.
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    void initialize_after_surface_creation(VkSurfaceKHR surface);

    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

    [[nodiscard]] VkFormat findDepthFormat() const;

    VkQueue graphicsQueue{};

private:
    bool initialized = false;
    // There is no global state in Vulkan and all per-application state is stored in a VkInstance object.
    // Creating a VkInstance object initializes the Vulkan library and allows the application to pass
    // information about itself to the implementation.
    VkInstance instance{};

    VkDebugUtilsMessengerEXT debugMessenger{};

    /// Logical device.
    VkDevice device{};

private:
    /// Initialize the Vulkan library by creating an instance.
    /// The instance is the connection between your application and the Vulkan library.
    void create_instance();
    void setupDebugMessenger();

    void create_command_pool(VkSurfaceKHR surface);
    static bool checkDeviceExtensionSupport(VkPhysicalDevice pPhysicalDevice);

    void pick_physical_device(VkSurfaceKHR surface);

    void create_logical_device(VkSurfaceKHR surface);

    /**
     * Check if a physical device is suitable.
     * @param pPhysicalDevice
     * @return
     */
    static bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    [[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                               VkImageTiling tiling,
                                               VkFormatFeatureFlags features) const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData) {
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    static bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : validationLayers) {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    static std::vector<const char *> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
};

} // namespace Flint

#endif // FLINT_DISPLAY_SERVER_H
