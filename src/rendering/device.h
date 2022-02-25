//
// Created by chy on 2/16/2022.
//

#ifndef FLINT_DEVICE_H
#define FLINT_DEVICE_H

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <iostream>
#include <vector>
#include <optional>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

/// How many frames should be processed concurrently.
const int MAX_FRAMES_IN_FLIGHT = 2;

/// List of required validation layers.
const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

/// List of required device extensions.
const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

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

class Device {
public:
    Device();

    static Device &getSingleton() {
        static Device singleton;
        return singleton;
    }

    GLFWwindow *window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkSurfaceKHR surface;

    /// The graphics card that we'll end up selecting will be stored in a VkPhysicalDevice handle.
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    /// Logical device.
    VkDevice device{};

    static const bool enableValidationLayers = true;

    void initWindow();

    bool framebufferResized = false;

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto app = reinterpret_cast<Device *>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void createInstance();

    void createSurface();

    bool checkDeviceExtensionSupport(VkPhysicalDevice pPhysicalDevice) const;

    /**
     * Check if a physical device is suitable.
     * @param pPhysicalDevice
     * @return
     */
    bool isDeviceSuitable(VkPhysicalDevice pPhysicalDevice) const;

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

    void pickPhysicalDevice();

    void createLogicalDevice();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

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

        for (const char *layerName: validationLayers) {
            bool layerFound = false;

            for (const auto &layerProperties: availableLayers) {
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

    void setupDebugMessenger();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice pPhysicalDevice) const;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice pPhysicalDevice) const;

    [[nodiscard]] VkFormat findDepthFormat() const;

    [[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                               VkImageTiling tiling,
                                               VkFormatFeatureFlags features) const;

    void cleanup();
};

#endif //FLINT_DEVICE_H
