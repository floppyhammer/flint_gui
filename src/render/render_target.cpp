#include "render_target.h"

#include <array>

namespace Flint {
RenderTarget::RenderTarget() {
    // Render pass is not extent dependent.
    create_render_pass();

    extent_dependent_init();
}

RenderTarget::~RenderTarget() {
    auto device = Platform::getSingleton()->device;

    extent_dependent_cleanup();

    vkDestroyRenderPass(device, renderPass, nullptr);
}

void RenderTarget::create_images() {
    // Color.
    texture = ImageTexture::from_empty(extent, VK_FORMAT_R8G8B8A8_UNORM);

    // Depth.
    VkFormat depthFormat = Platform::getSingleton()->findDepthFormat();
    RenderServer::getSingleton()->createImage(extent.x,
                                              extent.y,
                                              depthFormat,
                                              VK_IMAGE_TILING_OPTIMAL,
                                              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                              depthImage,
                                              depthImageMemory);
    depthImageView = RenderServer::getSingleton()->createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void RenderTarget::create_render_pass() {
    // Color attachment.
    // ----------------------------------------
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format =
        VK_FORMAT_R8G8B8A8_UNORM; // Specifying the format of the image view that will be used for the attachment.
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // Specifying the number of samples of the image.
    colorAttachment.loadOp =
        VK_ATTACHMENT_LOAD_OP_CLEAR; // Specifying how the contents of color and depth components of the attachment are
                                     // treated at the beginning of the subpass where it is first used.
    colorAttachment.storeOp =
        VK_ATTACHMENT_STORE_OP_STORE; // Specifying how the contents of color and depth components of the attachment are
                                      // treated at the end of the subpass where it is last used.
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // The layout the attachment image subresource will be in
                                                               // when a render pass instance begins.
    colorAttachment.finalLayout =
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // The layout the attachment image subresource will be transitioned to
                                                  // when a render pass instance ends.

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Specifying the layout the attachment uses during the subpass.
    // ----------------------------------------

    // Depth attachment.
    // ----------------------------------------
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = Platform::getSingleton()->findDepthFormat();
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

    // Use subpass dependencies for layout transitions.
    // ----------------------------------------
    std::array<VkSubpassDependency, 2> dependencies{};

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    // ----------------------------------------

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    // Create the actual render pass.
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    if (vkCreateRenderPass(Platform::getSingleton()->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}

void RenderTarget::create_framebuffer() {
    // Create framebuffer.
    {
        std::array<VkImageView, 2> attachments = {texture->imageView, depthImageView};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.x;
        framebufferInfo.height = extent.y;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(Platform::getSingleton()->device, &framebufferInfo, nullptr, &framebuffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }

    // Fill a descriptor for later use in a descriptor set.
    descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptor.imageView = texture->imageView;
    descriptor.sampler = texture->sampler;
}

VkRenderPassBeginInfo RenderTarget::getRenderPassInfo() {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer; // Set target framebuffer.
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VkExtent2D{(uint32_t)extent.x, (uint32_t)extent.y};

    return renderPassInfo;
}

void RenderTarget::create_pipelines() {
    // We need to create pipelines exclusively for this sub-viewport as pipelines contain render pass info.
    RenderServer::getSingleton()->createMeshPipeline(
        renderPass, VkExtent2D{(uint32_t)extent.x, (uint32_t)extent.y}, meshGraphicsPipeline);

    RenderServer::getSingleton()->createBlitPipeline(
        renderPass, VkExtent2D{(uint32_t)extent.x, (uint32_t)extent.y}, blitGraphicsPipeline);

    RenderServer::getSingleton()->create_skybox_pipeline(
        renderPass, VkExtent2D{(uint32_t)extent.x, (uint32_t)extent.y}, skybox_graphics_pipeline);
}

void RenderTarget::set_extent(Vec2I _extent) {
    if (extent != _extent) {
        extent = _extent;
        extent_dependent_cleanup();
        extent_dependent_init();
    }
}

Vec2I RenderTarget::get_extent() {
    return extent;
}

void RenderTarget::extent_dependent_init() {
    // Create color & depth images.
    create_images();

    create_framebuffer();

    create_pipelines();
}

void RenderTarget::extent_dependent_cleanup() const {
    auto device = Platform::getSingleton()->device;

    vkDestroyPipeline(device, meshGraphicsPipeline, nullptr);
    vkDestroyPipeline(device, blitGraphicsPipeline, nullptr);

    // Depth resources.
    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);

    vkDestroyFramebuffer(device, framebuffer, nullptr);
}
} // namespace Flint
