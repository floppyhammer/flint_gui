#ifndef FLINT_RENDER_TARGET_H
#define FLINT_RENDER_TARGET_H

#include "../common/math/vec2.h"
#include "../common/color.h"
#include "render_server.h"
#include "../resources/image_texture.h"

namespace Flint {
    /**
     * RenderTarget is basically a texture to which you can draw stuff.
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
        // ------------------------------------

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkRenderPass renderPass;

        VkFramebuffer framebuffer;

        VkDescriptorImageInfo descriptor;

        /**
         * When extent is changed, we need to recreate some Vulkan resources.
         * @param p_extent New extent
         */
        void set_extent(Vec2<uint32_t> p_extent);

        Vec2<uint32_t> get_extent();

    private:
        Vec2<uint32_t> extent = {512, 512};

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
}

#endif //FLINT_RENDER_TARGET_H
