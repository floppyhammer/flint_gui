#include <stdexcept>

#include "image_texture.h"
#include "../common/logger.h"

//#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

ImageTexture::~ImageTexture() {
    if (!resource_ownership) return;

    auto device = Platform::getSingleton().device;

    vkDestroySampler(device, sampler, nullptr);

    // Should be right before destroying the image itself.
    vkDestroyImageView(device, imageView, nullptr);

    // Release GPU memory.
    vkDestroyImage(device, image, nullptr);

    // Release CPU memory.
    vkFreeMemory(device, imageMemory, nullptr);
}

void ImageTexture::create_image_from_bytes(void *pixels, uint32_t tex_width, uint32_t tex_height) {
    width = tex_width;
    height = tex_height;

    // In bytes. 4 bytes per pixel.
    VkDeviceSize imageSize = tex_width * tex_height * 4;

    // Temporary buffer and CPU memory.
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    RenderServer::getSingleton().createBuffer(imageSize,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                              stagingBuffer,
                                              stagingBufferMemory);

    // Copy the pixel values that we got from the image loading library to the buffer.
    RenderServer::getSingleton().copyDataToMemory(pixels, stagingBufferMemory, imageSize);

    RenderServer::getSingleton().createImage(width, height,
                                             VK_FORMAT_R8G8B8A8_UNORM,
                                             VK_IMAGE_TILING_OPTIMAL,
                                             VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                             image,
                                             imageMemory);

    // Transition the texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
    RenderServer::getSingleton().transitionImageLayout(image,
                                                       VK_FORMAT_R8G8B8A8_UNORM,
                                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Execute the buffer to image copy operation.
    RenderServer::getSingleton().copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(width),
                                                   static_cast<uint32_t>(height));

    // To be able to start sampling from the texture image in the shader, we need one last transition to prepare it for shader access.
    RenderServer::getSingleton().transitionImageLayout(image,
                                                       VK_FORMAT_R8G8B8A8_UNORM,
                                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Clean up staging stuff.
    vkDestroyBuffer(Platform::getSingleton().device, stagingBuffer, nullptr);
    vkFreeMemory(Platform::getSingleton().device, stagingBufferMemory, nullptr);
}

std::shared_ptr<ImageTexture> ImageTexture::from_empty(uint32_t p_width, uint32_t p_height) {
    assert(p_width != 0 && p_height != 0 && "Creating texture with zero size.");

    auto texture = std::make_shared<ImageTexture>();
    texture->width = p_width;
    texture->height = p_height;

    // Pixel data.
    std::vector<unsigned char> pixels(p_width * p_height * 4, 0);

    // Create image and image memory.
    texture->create_image_from_bytes(pixels.data(), p_width, p_height);

    // Create image view.
    texture->imageView = RenderServer::getSingleton().createImageView(texture->image,
                                                                      VK_FORMAT_R8G8B8A8_UNORM,
                                                                      VK_IMAGE_ASPECT_COLOR_BIT);

    // Create sampler.
    RenderServer::getSingleton().createTextureSampler(texture->sampler);

    return texture;
}

ImageTexture::ImageTexture(const std::string &path) : Texture(path) {
    // The STBI_rgb_alpha value forces the image to be loaded with an alpha channel,
    // even if it doesn't have one, which is nice for consistency with other textures in the future.
    int tex_width, tex_height, tex_channels;
    stbi_uc *pixels = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

    if (!pixels) {
        Flint::Logger::warn("Failed to load image file " + path, "Texture");
        //return Texture::from_empty(4, 4);
        throw std::runtime_error("Failed to load texture image!");
    }

    // Create image and image memory.
    create_image_from_bytes(pixels, tex_width, tex_height);

    // Clean up the original pixel array.
    stbi_image_free(pixels);

    // Create image view.
    imageView = RenderServer::getSingleton().createImageView(image,
                                                             VK_FORMAT_R8G8B8A8_UNORM,
                                                             VK_IMAGE_ASPECT_COLOR_BIT);

    // Create sampler.
    RenderServer::getSingleton().createTextureSampler(sampler);
}

std::shared_ptr<ImageTexture> ImageTexture::from_wrapper(VkImageView p_image_view,
                                                         VkSampler p_sampler,
                                                         uint32_t p_width,
                                                         uint32_t p_height) {
    auto texture = std::make_shared<ImageTexture>();
    texture->width = p_width;
    texture->height = p_height;
    texture->resource_ownership = false;
    texture->imageView = p_image_view;
    texture->sampler = p_sampler;

    return texture;
}
