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
    explicit Texture(const std::string& filePath);

    uint32_t width, height;
    VkFormat format;

    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;

    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
    VkSampler sampler;

    void createTextureImage(void *pixels, int texWidth, int texHeight);

    void cleanup() const;
};

#endif //VULKAN_DEMO_APP_TEXTURE_H
