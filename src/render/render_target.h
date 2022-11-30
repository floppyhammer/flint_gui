#ifndef FLINT_RENDER_TARGET_H
#define FLINT_RENDER_TARGET_H

#include "../common/geometry.h"
#include "../resources/image_texture.h"
#include "render_server.h"

using Pathfinder::Transform2;
using Pathfinder::Vec2I;

namespace Flint {
/**
 * RenderTarget is basically a texture to which you can draw things.
 */
class RenderTarget {
public:
    RenderTarget();

    ~RenderTarget();

    VkRenderPassBeginInfo getRenderPassInfo();

    std::shared_ptr<ImageTexture> texture;

    // Pipelines bound with the render pass of this sub viewport.
    // ------------------------------------
    VkPipeline meshGraphicsPipeline;
    VkPipeline blitGraphicsPipeline;
    VkPipeline skybox_graphics_pipeline;
    // ------------------------------------

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkRenderPass renderPass;

    VkFramebuffer framebuffer;

    /// For later use of the image as a sampler target.
    VkDescriptorImageInfo descriptor;

    /**
     * When extent is changed, we need to recreate some Vulkan resources.
     * @param p_extent New extent
     */
    void set_extent(Vec2I _extent);

    Vec2I get_extent();

private:
    Vec2I extent = {512, 512};

    // Render pass doesn't care about extent.
    void create_render_pass();

    void extent_dependent_init();

    void extent_dependent_cleanup() const;

    void create_images();

    // The color attachment of this framebuffer will then be used
    // to sample from in the fragment shader of the final pass.
    void create_framebuffer();

    void create_pipelines();
};
} // namespace Flint

#endif // FLINT_RENDER_TARGET_H
