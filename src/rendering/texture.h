//
// Created by tannh on 2/13/2022.
//

#ifndef VULKAN_DEMO_APP_TEXTURE_H
#define VULKAN_DEMO_APP_TEXTURE_H

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

class Texture {
public:
    uint32_t width, height;
    VkFormat format;

    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;

    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;

    void createImage();
};

#endif //VULKAN_DEMO_APP_TEXTURE_H
