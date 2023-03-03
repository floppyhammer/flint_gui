#ifndef FLINT_SUBVIEW_H
#define FLINT_SUBVIEW_H

#include "../common/geometry.h"
#include "../resources/image_texture.h"
#include "servers/render_server.h"

namespace Flint {

/// Subview is a texture to which you can draw things.
/// It is used with a camera to create a viewport.
class Subview {
public:
    explicit Subview(Vec2I view_size);

    ~Subview();

    VkRenderPassBeginInfo get_render_pass_info();

    std::shared_ptr<ImageTexture> texture;

    /// For later use of the image as a sampler target.
    VkDescriptorImageInfo descriptor;

    VkImage depth_image;
    VkDeviceMemory depth_image_memory;
    VkImageView depth_image_view;

    VkRenderPass render_pass;

    VkFramebuffer framebuffer;

    /// When extent is changed, we need to recreate some Vulkan resources.
    void set_extent(Vec2I new_extent);

    Vec2I get_extent();

    Vec2I extent = {128, 128};

private:
    // Render pass doesn't care about extent.
    void create_render_pass();

    void create_extent_dependent_resources();

    void create_images();

    // The color attachment of this framebuffer will then be used
    // to sample from in the fragment shader of the final pass.
    void create_framebuffer();

    void destroy_extent_dependent_resources() const;
};

} // namespace Flint

#endif // FLINT_SUBVIEW_H
