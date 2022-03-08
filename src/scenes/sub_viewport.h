#ifndef FLINT_SUB_VIEWPORT_H
#define FLINT_SUB_VIEWPORT_H

#include "node.h"
#include "../common/vec2.h"
#include "../rendering/rendering_server.h"
#include "../core/scene_tree.h"

namespace Flint {
    /**
     * SubViewport is not a node.
     */
    class SubViewport : public Node {
    public:
        ~SubViewport();

        Vec2<uint32_t> extent = {512, 512};

        float fov = 45.0;

        float z_near = 0.1;
        float z_far = 10.0;

        void prepare();

        std::shared_ptr<SceneTree> tree;

        void draw() override;

    public:
        VkPipeline meshInstance3dGraphicsPipeline;

    protected:
        void update(double delta) override;

        // Should have the same number as swap chain images.
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;

        // How should this sub viewport be sampled.
        VkSampler sampler;

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
