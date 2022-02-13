//
// Created by tannh on 2/13/2022.
//

#ifndef VULKAN_DEMO_APP_TEXTURE_H
#define VULKAN_DEMO_APP_TEXTURE_H

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include "rendering_server.h"

#include <memory>

class Texture {
public:
    /// Load a texture from directory.
    explicit Texture(const std::string &filename);

    uint32_t width = 0;
    uint32_t height = 0;

    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

    VkImage image{};
    VkDeviceMemory imageMemory{};
    VkImageView imageView;
    VkSampler sampler{};

    void createImageFromBytes(void *pixels, int texWidth, int texHeight);

    void cleanup() const;
};

#endif //VULKAN_DEMO_APP_TEXTURE_H
