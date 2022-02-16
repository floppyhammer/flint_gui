#include <stdexcept>

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

Texture::Texture(const std::string &filename) {
    // The STBI_rgb_alpha value forces the image to be loaded with an alpha channel,
    // even if it doesn't have one, which is nice for consistency with other textures in the future.
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels) {
        throw std::runtime_error("Failed to load texture image!");
    }

    // Create image and image memory.
    createImageFromBytes(pixels, texWidth, texHeight);

    // Clean up the original pixel array.
    stbi_image_free(pixels);

    // Create image view.
    imageView = RS::getSingleton().createImageView(image,
                                                   VK_FORMAT_R8G8B8A8_SRGB,
                                                   VK_IMAGE_ASPECT_COLOR_BIT);

    // Create sampler.
    RS::getSingleton().createTextureSampler(sampler);
}

Texture::~Texture() {
    cleanup();
}

void Texture::createImageFromBytes(void *pixels, int texWidth, int texHeight) {
    width = texWidth;
    height = texHeight;

    // In bytes. 4 bytes per pixel.
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    // Temporary buffer and CPU memory.
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    RS::getSingleton().createBuffer(imageSize,
                                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    stagingBuffer,
                                    stagingBufferMemory);

    // Copy the pixel values that we got from the image loading library to the buffer.
    RS::getSingleton().copyDataToMemory(pixels, stagingBufferMemory, imageSize);

    RS::getSingleton().createImage(texWidth, texHeight,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_TILING_OPTIMAL,
                                   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                   image,
                                   imageMemory);

    // Transition the texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
    RS::getSingleton().transitionImageLayout(image,
                                             VK_FORMAT_R8G8B8A8_SRGB,
                                             VK_IMAGE_LAYOUT_UNDEFINED,
                                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Execute the buffer to image copy operation.
    RS::getSingleton().copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth),
                                         static_cast<uint32_t>(texHeight));

    // To be able to start sampling from the texture image in the shader, we need one last transition to prepare it for shader access.
    RS::getSingleton().transitionImageLayout(image,
                                             VK_FORMAT_R8G8B8A8_SRGB,
                                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Clean up staging stuff.
    vkDestroyBuffer(RS::getSingleton().device, stagingBuffer, nullptr);
    vkFreeMemory(RS::getSingleton().device, stagingBufferMemory, nullptr);
}

void Texture::cleanup() const {
    auto device = RS::getSingleton().device;

    vkDestroySampler(device, sampler, nullptr);

    // Should be right before destroying the image itself.
    vkDestroyImageView(device, imageView, nullptr);

    // Release GPU memory.
    vkDestroyImage(device, image, nullptr);

    // Release CPU memory.
    vkFreeMemory(device, imageMemory, nullptr);
}
