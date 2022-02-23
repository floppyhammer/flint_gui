#ifndef FLINT_H
#define FLINT_H

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
#include "rendering/texture.h"
#include "scenes/3d/mesh_instance_3d.h"
#include "core/scene_tree.h"

class App {
public:
    void run();

    Flint::SceneTree tree;

private:
    VkSwapchainKHR swapChain;

    /// VkImage defines which VkMemory is used and a format of the texel.
    /// Number of images doesn't necessarily equal to MAX_FRAMES_IN_FLIGHT (One is expected, the other is what we actually get considering device capacity).
    std::vector<VkImage> swapChainImages;

    /// We only need a single depth image unlike the swap chain images, because only one draw operation is running at once.
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    /// Store the format and extent we've chosen for the swap chain images.
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    /// VkImageView defines which part of VkImage to use.
    std::vector<VkImageView> swapChainImageViews;

    /// VkFramebuffer + VkRenderPass defines the render target.
    /// Render pass defines which attachment will be written with colors.
    /// VkFramebuffer defines which VkImageView is to be which attachment.
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;

    std::vector<VkCommandBuffer> commandBuffers;

    /// Each frame should have its own set of semaphores, so a list is used.
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;

    /// To perform CPU-GPU synchronization using fences.
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    /// To use the right pair of semaphores every time,
    /// we need to keep track of the current frame.
    size_t currentFrame = 0;

    void initSwapChain();

    void mainLoop();

    void recreateSwapChain();

    /**
     * What have been created in createSwapChainRelatedResources() have to be destroyed in here.
     */
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
     * @dependency Swap chain image format.
     */
    void createRenderPass();

    /**
     *
     * @dependency Swap chain extent, render pass, swap chain image views.
     */
    void createFramebuffers();

    /**
     * Set up command queues.
     * @dependency None.
     */
    void createCommandBuffers();

    void createSyncObjects();

    void drawFrame();

    void createSwapChainRelatedResources();

    /**
     * Record commands into the current command buffer.
     * @dependency Render pass, swap chain framebuffers, graphics pipeline, vertex buffer, index buffer, pipeline layout.
     */
    void recordCommands(uint32_t imageIndex);

    bool acquireSwapChainImage(uint32_t& imageIndex);

    void flush(uint32_t imageIndex);
};

#endif //FLINT_H
