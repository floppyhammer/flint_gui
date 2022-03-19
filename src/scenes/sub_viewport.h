#ifndef FLINT_SUB_VIEWPORT_H
#define FLINT_SUB_VIEWPORT_H

#include "node.h"
#include "../common/vec2.h"
#include "../rendering/rendering_server.h"
#include "../rendering/texture.h"
#include "../core/scene_tree.h"

namespace Flint {
    class SubViewport : public Node {
    public:
        SubViewport();
        ~SubViewport();

        Vec2<uint32_t> extent = {512, 512};

        float fov = 45.0;

        float z_near = 0.1;
        float z_far = 10.0;

        void prepare();

        std::shared_ptr<SceneTree> tree;

        void draw() override;

        std::shared_ptr<Texture> texture;
    public:
        // Pipelines bound with the render pass of this sub viewport.
        // ------------------------------------
        VkPipeline meshGraphicsPipeline;
        VkPipeline blitGraphicsPipeline;
        // ------------------------------------

    protected:
        void update(double delta) override;

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

#endif //FLINT_SUB_VIEWPORT_H
