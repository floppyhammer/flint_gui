#ifndef VULKAN_DEMO_APP_H
#define VULKAN_DEMO_APP_H

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include "glm/glm.hpp"

#include <vector>
#include <optional>
#include <iostream>
#include <fstream>
#include <array>

#include <cstring>

#include "rendering/mesh.h"
#include "rendering//texture.h"

// MVP, which will be sent to vertex shaders.
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class App {
public:
    void run();

private:
    VkSurfaceKHR surface;

    // The graphics card that we'll end up selecting will be stored in a VkPhysicalDevice handle.
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    // Logical device.
    VkDevice device{};

    VkSwapchainKHR swapChain;

    // VkImage defines which VkMemory is used and a format of the texel.
    std::vector<VkImage> swapChainImages;

    // We only need a single depth image, because only one draw operation is running at once.
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    // Store the format and extent we've chosen for the swap chain images.
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    // VkImageView defines which part of VkImage to use.
    std::vector<VkImageView> swapChainImageViews;

    // VkFramebuffer + VkRenderPass defines the render target.
    // Render pass defines which attachment will be written with colors.
    // VkFramebuffer defines which VkImageView is to be which attachment.
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    std::vector<VkCommandBuffer> commandBuffers;

    // Contains vertices and indices data.
    Mesh mesh;

    // Vertex buffer.
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    // Index buffer.
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    // We have a uniform buffer per swap chain image.
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    // Each frame should have its own set of semaphores, so a list is used.
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;

    // To perform CPU-GPU synchronization using fences.
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    // To use the right pair of semaphores every time,
    // we need to keep track of the current frame.
    // We will use a frame index for that purpose:
    size_t currentFrame = 0;

    // For model texture.
    std::shared_ptr<Texture> texture;

    void initVulkan();

    void mainLoop();

    /**
     * Update MVP.
     * @param currentImage Current image.
     */
    void updateUniformBuffer(uint32_t currentImage);

    void recreateSwapChain();

    void cleanupSwapChain();

    void cleanup();

    /**
     * Vulkan does not use the idea of a "back buffer". So, we need a place to render into
     * before moving an image to viewing. This place is called the Swap Chain.
     *
     * In essence, the Swap Chain manages one or more image objects that
     * form a sequence of images that can be drawn into and then given to
     * the Surface to be presented to the user for viewing.
     */
    void createSwapChain();

    void createImageViews();

    void createDepthResources();

    /**
     * We need to tell Vulkan about the framebuffer attachments that
     * will be used while rendering. We need to specify how many
     * color and depth buffers there will be, how many samples to
     * use for each of them and how their contents should be
     * handled throughout the rendering operations. All of this
     * information is wrapped in a render pass object.
     */
    void createRenderPass();

    /**
     * Set up shaders, viewport, blend state, etc.
     */
    void createGraphicsPipeline();

    void createFramebuffers();

    void createVertexBuffer();

    void createIndexBuffer();

    void createUniformBuffers();

    /**
     * A descriptor pool is used to allocate descriptor sets of some layout for use in a shader.
     */
    void createDescriptorPool();

    void createDescriptorSets();

    // Set up command queues.
    void createCommandBuffers();

    void createSyncObjects();

    void drawFrame();

    /// Create UBO descriptor.
    void createDescriptorSetLayout();

    void loadModel();
};

#endif //VULKAN_DEMO_APP_H
