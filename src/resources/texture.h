#ifndef FLINT_TEXTURE_H
#define FLINT_TEXTURE_H

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include "../rendering/rendering_server.h"

#include <memory>

class Texture {
public:
    /// Create an empty texture with specific size.
    static std::shared_ptr<Texture> create(uint32_t p_width, uint32_t p_height);

    /// Create a texture from file.
    static std::shared_ptr<Texture> from_file(const std::string &filename);

    Texture() = default;

    ~Texture();

public:
    uint32_t width = 0;
    uint32_t height = 0;

    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

    /// GPU memory.
    VkImage image{};

    /// CPU memory.
    VkDeviceMemory imageMemory{};

    /// Thin wrapper.
    VkImageView imageView{};

    /// How image should be filtered.
    VkSampler sampler{};

private:
    void create_image_from_bytes(void *pixels, uint32_t tex_width, uint32_t tex_height);
};

#endif //FLINT_TEXTURE_H
