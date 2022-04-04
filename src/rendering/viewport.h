#ifndef FLINT_VIEWPORT_H
#define FLINT_VIEWPORT_H

#include "../common/vec2.h"
#include "../common/color.h"
#include "rendering_server.h"
#include "texture.h"

namespace Flint {
    class Viewport {
    public:
        Viewport();

        ~Viewport();

        VkRenderPassBeginInfo &getRenderPassInfo();

        Vec2<uint32_t> extent = {512, 512};

        std::shared_ptr<Texture> texture;

    public:
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

        void createImages();

        // The color attachment of this framebuffer will then be used
        // to sample from in the fragment shader of the final pass.
        void createRenderPass();

        void createFramebuffer();
    };
}

#endif //FLINT_VIEWPORT_H
