#include "subview.h"

#include <array>

namespace Flint {

Subview::Subview(Vec2I view_size) {
    extent = view_size;

    // Render pass is not extent dependent.
    create_render_pass();

    extent_dependent_init();
}

Subview::~Subview() {
    auto device = DisplayServer::get_singleton()->get_device();

    extent_dependent_cleanup();

    vkDestroyRenderPass(device, render_pass, nullptr);
}

void Subview::create_images() {
    // Color.
    texture = ImageTexture::from_empty(extent, VK_FORMAT_R8G8B8A8_UNORM);

    auto rs = RenderServer::get_singleton();

    // A render target image's layout will always be SHADER_READ_ONLY.
    auto cmd_buffer = rs->beginSingleTimeCommands();
    RenderServer::transitionImageLayout(cmd_buffer,
                                        texture->image,
                                        texture->format,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                        1,
                                        1);
    rs->endSingleTimeCommands(cmd_buffer);

    // Depth.
    VkFormat depthFormat = DisplayServer::get_singleton()->findDepthFormat();
    rs->createImage(extent.x,
                    extent.y,
                    depthFormat,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    depth_image,
                    depth_image_memory);
    depth_image_view = rs->createImageView(depth_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void Subview::create_render_pass() {
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
    colorAttachment.initialLayout =
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // The layout the attachment image subresource will be in
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

    if (vkCreateRenderPass(DisplayServer::get_singleton()->get_device(), &renderPassInfo, nullptr, &render_pass) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}

void Subview::create_framebuffer() {
    // Create framebuffer.
    {
        std::array<VkImageView, 2> attachments = {texture->imageView, depth_image_view};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = render_pass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.x;
        framebufferInfo.height = extent.y;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
                DisplayServer::get_singleton()->get_device(), &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }

    // Fill a descriptor for later use in a descriptor set.
    descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptor.imageView = texture->imageView;
    descriptor.sampler = texture->sampler;
}

VkRenderPassBeginInfo Subview::get_render_pass_info() {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = render_pass;
    renderPassInfo.framebuffer = framebuffer; // Set target framebuffer.
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VkExtent2D{(uint32_t)extent.x, (uint32_t)extent.y};

    return renderPassInfo;
}

void Subview::set_extent(Vec2I new_extent) {
    if (extent != new_extent) {
        extent = new_extent;
        extent_dependent_cleanup();
        extent_dependent_init();
    }
}

Vec2I Subview::get_extent() {
    return extent;
}

void Subview::extent_dependent_init() {
    // Create color & depth images.
    create_images();

    create_framebuffer();
}

void Subview::extent_dependent_cleanup() const {
    auto device = DisplayServer::get_singleton()->get_device();

    // Depth resources.
    vkDestroyImageView(device, depth_image_view, nullptr);
    vkDestroyImage(device, depth_image, nullptr);
    vkFreeMemory(device, depth_image_memory, nullptr);

    vkDestroyFramebuffer(device, framebuffer, nullptr);
}

} // namespace Flint
