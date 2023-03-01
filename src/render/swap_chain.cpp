#include "swap_chain.h"

#include <array>
#include <cstdint>
#include <memory>

#include "servers/display_server.h"
#include "servers/render_server.h"

namespace Flint {

SwapChain::SwapChain(Window* _window) {
    // Record parent window.
    window = _window;

    // Swap chain related resources.
    initSwapChain();

    createSyncObjects();
}

SwapChain::~SwapChain() {
    cleanup();
}

void SwapChain::initSwapChain() {
    // Create a swap chain and corresponding swap chain images.
    createSwapChain();

    // Create views for swap chain images.
    createImageViews();

    createRenderPass();

    createDepthResources();

    createFramebuffers();

    // Create a command buffer for each swap chain image.
    createCommandBuffers();

    window->create_swapchain_related_resources(renderPass, swapChainExtent);
}

void SwapChain::recreateSwapChain() {
    // Handling window minimization.
    int width = 0, height = 0;
    glfwGetFramebufferSize(window->glfw_window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window->glfw_window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(DisplayServer::get_singleton()->get_device());

    cleanupSwapChain();

    initSwapChain();

    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
}

void SwapChain::cleanupSwapChain() {
    auto device = DisplayServer::get_singleton()->get_device();

    // Command buffers contain swap chain related info, so we also need to free them here.
    window->cleanup_swapchain_related_resources();

    // Depth resources.
    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);

    // Framebuffers.
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    // Only command buffers are freed but not the pool.
    vkFreeCommandBuffers(device,
                         DisplayServer::get_singleton()->command_pool,
                         static_cast<uint32_t>(commandBuffers.size()),
                         commandBuffers.data());

    vkDestroyRenderPass(device, renderPass, nullptr);

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void SwapChain::cleanup() {
    auto device = DisplayServer::get_singleton()->get_device();

    // Clean up swap chain related resources.
    cleanupSwapChain();

    // Clean up sync objects.
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }
}

void SwapChain::createSwapChain() {
    auto device = DisplayServer::get_singleton()->get_device();

    SwapChainSupportDetails swapChainSupport = DisplayServer::get_singleton()->querySwapChainSupport(
        DisplayServer::get_singleton()->physicalDevice, window->surface);

    VkSurfaceFormatKHR surfaceFormat = window->chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = window->chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = window->chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = window->surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices qfIndices = DisplayServer::get_singleton()->findQueueFamilies(
        DisplayServer::get_singleton()->physicalDevice, window->surface);
    uint32_t queueFamilyIndices[] = {qfIndices.graphicsFamily.value(), qfIndices.presentFamily.value()};

    if (qfIndices.graphicsFamily != qfIndices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // Create a swapchain.
    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain!");
    }

    // Get the number of presentable images for swapchain.
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);

    // Obtain the array of presentable images associated with a swapchain.
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void SwapChain::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = RenderServer::get_singleton()->createImageView(
            swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void SwapChain::createDepthResources() {
    auto rs = RenderServer::get_singleton();

    VkFormat depthFormat = DisplayServer::get_singleton()->findDepthFormat();
    rs->createImage(swapChainExtent.width,
                    swapChainExtent.height,
                    depthFormat,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    depthImage,
                    depthImageMemory);
    depthImageView = rs->createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    auto cmd_buffer = rs->beginSingleTimeCommands();

    RenderServer::transitionImageLayout(cmd_buffer,
                                        depthImage,
                                        depthFormat,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    rs->endSingleTimeCommands(cmd_buffer);
}

void SwapChain::createRenderPass() {
    // Color attachment.
    // ----------------------------------------
    VkAttachmentDescription colorAttachment{};
    // Specifying the format of the image view that will be used for the attachment.
    colorAttachment.format = swapChainImageFormat;
    // Specifying the number of samples of the image.
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // Specifying how the contents of color and depth components of the attachment are
    // treated at the beginning of the subpass where it is first used.
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // Specifying how the contents of color and depth components of the attachment are
    // treated at the end of the subpass where it is last used.
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // The layout the attachment image subresource will be in
    // when a render pass instance begins.
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // The layout the attachment image subresource will be transitioned to when a
    // render pass instance ends.
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    // Specifying the layout the attachment uses during the subpass.
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // ----------------------------------------

    // Depth attachment.
    // ----------------------------------------
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = DisplayServer::get_singleton()->findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    // ----------------------------------------

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(DisplayServer::get_singleton()->get_device(), &renderPassInfo, nullptr, &renderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}

void SwapChain::createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {swapChainImageViews[i], depthImageView};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
                DisplayServer::get_singleton()->get_device(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

void SwapChain::createSyncObjects() {
    auto device = DisplayServer::get_singleton()->get_device();

    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Initialize it in the signaled state

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create synchronization objects for a frame!");
        }
    }
}

bool SwapChain::acquireSwapChainImage(uint32_t& imageIndex) {
    auto device = DisplayServer::get_singleton()->get_device();

    // Wait for the frame to be finished.
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    // Retrieve the index of the next available presentable image.
    VkResult result = vkAcquireNextImageKHR(
        device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    currentImage = imageIndex;

    // Recreate swap chains if necessary.
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    return true;
}

void SwapChain::createCommandBuffers() {
    commandBuffers.resize(swapChainFramebuffers.size());

    // Allocate command buffers.
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = DisplayServer::get_singleton()->command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(DisplayServer::get_singleton()->get_device(), &allocInfo, commandBuffers.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

void SwapChain::flush(uint32_t imageIndex) {
    auto device = DisplayServer::get_singleton()->get_device();

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    // Submit command buffer.
    // -------------------------------------
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    // The semaphores to signal after all commands in the buffer are finished.
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(DisplayServer::get_singleton()->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }
    // -------------------------------------

    // Queue an image for presentation after queueing all rendering commands and transitioning the image to the correct
    // layout.
    // -------------------------------------
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Specifies the semaphores to wait for before issuing the present request.
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    // Array of each swap chain’s presentable images.
    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(window->presentQueue, &presentInfo);
    // -------------------------------------

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->framebufferResized) {
        window->framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace Flint
