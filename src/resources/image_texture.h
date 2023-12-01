#ifndef FLINT_IMAGE_TEXTURE_H
#define FLINT_IMAGE_TEXTURE_H

#include <memory>

#include "../render/base.h"
#include "texture.h"

namespace Flint {

class ImageTexture : public Texture {
public:
    ImageTexture();

    /// Create a texture from file.
    explicit ImageTexture(const std::string &path);

    ~ImageTexture() override;

    /// Create an empty texture with specific size.
    static std::shared_ptr<ImageTexture> from_empty(Vec2I size, VkFormat format);

    static std::shared_ptr<ImageTexture> from_wrapping(VkImageView image_view, VkSampler sampler, Vec2I size);

    /// If this texture should take the responsibility of managing GPU resources.
    bool resource_ownership = true;

    void set_filter(VkFilter filter);

    std::shared_ptr<Pathfinder::Image> image_data;

public:
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

    /// Handle.
    VkImage image{};

    /// Device memory.
    VkDeviceMemory imageMemory{};

    /// Thin wrapper.
    VkImageView imageView{};

    /// How image should be filtered.
    VkSampler sampler{};

    VkImageLayout layout{};

private:
    void create_image_from_bytes(void *pixels, uint32_t tex_width, uint32_t tex_height, VkFormat tex_format);
};

} // namespace Flint

#endif // FLINT_IMAGE_TEXTURE_H
