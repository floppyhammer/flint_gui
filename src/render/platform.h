#ifndef FLINT_PLATFORM_H
#define FLINT_PLATFORM_H

#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <optional>
#include <vector>

#include "GLFW/glfw3.h"

/// How many frames should be processed concurrently.
const int MAX_FRAMES_IN_FLIGHT = 2;

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

class Platform {
public:
    Platform() = default;

    static Platform *getSingleton() {
        static Platform singleton;
        return &singleton;
    }

    void init(uint32_t window_width, uint32_t window_height);

    GLFWwindow *window{};

    /// Native platform surface or window objects are abstracted by surface objects.
    VkSurfaceKHR surface{};

    /// The graphics card that we'll end up selecting will be stored in a VkPhysicalDevice handle.
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    /// Logical device.
    VkDevice device{};

    /// Flag indicating the window size has changed.
    bool framebufferResized = false;

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        // We have attached a Platform pointer to this window previously. Now we retrieve it.
        auto platform = reinterpret_cast<Platform *>(glfwGetWindowUserPointer(window));
        platform->framebufferResized = true;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

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

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice pPhysicalDevice) const;

    VkQueue graphicsQueue{};
    VkQueue presentQueue{};

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice pPhysicalDevice) const;

    [[nodiscard]] VkFormat findDepthFormat() const;

    [[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                               VkImageTiling tiling,
                                               VkFormatFeatureFlags features) const;

    void cleanup();

private:
    // There is no global state in Vulkan and all per-application state is stored in a VkInstance object.
    // Creating a VkInstance object initializes the Vulkan library and allows the application to pass
    // information about itself to the implementation.
    VkInstance instance{};

    VkDebugUtilsMessengerEXT debugMessenger{};

    static const bool enableValidationLayers = true;

private:
    void initWindow(uint32_t window_width, uint32_t window_height);

    void setupDebugMessenger();

    void createInstance();

    void createSurface();

    bool checkDeviceExtensionSupport(VkPhysicalDevice pPhysicalDevice) const;

    /**
     * Check if a physical device is suitable.
     * @param pPhysicalDevice
     * @return
     */
    bool isDeviceSuitable(VkPhysicalDevice pPhysicalDevice) const;

    void pickPhysicalDevice();

    void createLogicalDevice();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
};

#endif // FLINT_PLATFORM_H
