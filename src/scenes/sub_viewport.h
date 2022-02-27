#ifndef FLINT_SUB_VIEWPORT_H
#define FLINT_SUB_VIEWPORT_H

#include "../common/vec2.h"
#include "../rendering/rendering_server.h"

namespace Flint {
    class SubViewport {
    public:
        Vec2<int> extent = {512, 512};

        float fov = 45.0;

        float z_near = 0.1;
        float z_far = 10.0;

    protected:
        // Should have the same number as swap chain images.
        std::vector<VkImage> images;
        std::vector<VkDeviceMemory> imagesMemory;
        std::vector<VkImageView> imageViews;

        VkRenderPass renderPass;

        void createImages();

        void createRenderPass();
    };
}

#endif //FLINT_SUB_VIEWPORT_H
