#ifndef FLINT_RENDERING_SERVER_H
#define FLINT_RENDERING_SERVER_H

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>

#include <vector>
#include <iostream>
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

/// MVP, which will be sent to vertex shaders.
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class RenderingServer {
public:
    static RenderingServer &getSingleton() {
        static RenderingServer singleton;
        return singleton;
    }

    // -----------------
    VkCommandBuffer p_commandBuffer;
    uint8_t p_whichBuffer;
    std::vector<VkImage> *p_swapChainImages;
    VkExtent2D p_swapChainExtent;
    uint32_t currentImage;
    // -----------------

    RenderingServer();

    void runAfterSwapchainCreation(VkRenderPass renderPass, VkExtent2D swapChainExtent);

    [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

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

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto app = reinterpret_cast<RenderingServer *>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    bool framebufferResized = false;

public:
    GLFWwindow *window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkSurfaceKHR surface;

    /// The graphics card that we'll end up selecting will be stored in a VkPhysicalDevice handle.
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    /// Logical device.
    VkDevice device{};

    static const bool enableValidationLayers = true;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkCommandPool commandPool;

public:
    void createInstance();

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    void createSurface();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void cleanup();

    void initWindow();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData) {
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void setupDebugMessenger();

    /**
     * Check if a physical device is suitable.
     * @param pPhysicalDevice
     * @return
     */
    bool isDeviceSuitable(VkPhysicalDevice pPhysicalDevice) const;

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice pPhysicalDevice) const;

    static bool checkDeviceExtensionSupport(VkPhysicalDevice pPhysicalDevice);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice pPhysicalDevice) const;

    [[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

    void createCommandPool();

    /**
     * Create a shader module, but the shader stage is not specified yet.
     * @return Shader module.
     */
    [[nodiscard]] VkShaderModule createShaderModule(const std::vector<char> &code) const;

    /**
     * Create a command buffer in the command pool, and start recording.
     * @return Command buffer.
     */
    [[nodiscard]] VkCommandBuffer beginSingleTimeCommands() const;

    /**
     * End recording, submit the command buffer to a queue, then free the command buffer in the command pool.
     */
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;

    /**
     * Copy data from VkBuffer to VkImage.
     * @param buffer Src buffer.
     * @param image Dst image.
     * @param width Image width.
     * @param height Image height.
     */
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

    static bool hasStencilComponent(VkFormat format);

    /**
     * Copy data from VkBuffer to VkBuffer.
     * @param srcBuffer Src buffer.
     * @param dstBuffer Dst buffer.
     * @param VkDeviceSize Data size in bytes.
     */
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) const;

    /**
     * An image view is a reference to a VkImage.
     * Unlike VkImage, it does not need to be allocated on GPU memory,
     * so you create them directly from the Vulkan API.
     * @param image
     * @param format
     * @param aspectFlags If for color or depth attachment.
     * @return
     */
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;

    /**
     * Create GPU buffer and CPU buffer memory and bind them.
     * @param size
     * @param usage
     * @param properties
     * @param buffer
     * @param bufferMemory
     */
    void createBuffer(VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory) const;

    /**
     * Copy data to device memory.
     * @param src Source data pointer.
     * @param bufferMemory Device memory.
     * @param dataSize Data size in bytes.
     */
    void copyDataToMemory(void *src, VkDeviceMemory bufferMemory, size_t dataSize) const;

    void createTextureSampler(VkSampler &textureSampler) const;

    [[nodiscard]] VkFormat findDepthFormat() const;

    [[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                               VkImageTiling tiling,
                                               VkFormatFeatureFlags features) const;

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

public:
    void draw_canvas_item();

public:
    // Texture rect.
    // --------------------------------------------------

    // --------------------------------------------------

    // Mesh instance 3D.
    // --------------------------------------------------
    // These should be shared by all mesh instances.
    VkDescriptorSetLayout meshInstance3dDescriptorSetLayout;
    VkPipelineLayout meshInstance3dPipelineLayout;
    VkPipeline meshInstance3dGraphicsPipeline;

    /**
     * Create UBO descriptor.
     * @dependency None.
     */
    void createMeshInstance3dDescriptorSetLayout();

    /**
     * Set up shaders, viewport, blend state, etc.
     * @note We only need one pipeline for a specific rendering process despite of the existence of multiple swap chains.
     * @dependency Descriptor set layout, swap chain extent.
     */
    void createMeshInstance3dGraphicsPipeline(VkRenderPass renderPass, VkExtent2D swapChainExtent);

    void draw_mesh_instance(VkDescriptorSet const &descriptorSet,
                            VkBuffer *vertexBuffers,
                            VkBuffer indexBuffer,
                            uint32_t indexCount) const;
    // --------------------------------------------------
};

typedef RenderingServer RS;

#endif //FLINT_RENDERING_SERVER_H
