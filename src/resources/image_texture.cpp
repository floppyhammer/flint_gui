#include "image_texture.h"

#include "../common/logger.h"
#include "../common/macros.h"

// Already defined in Pathfinder.
// #define STB_IMAGE_IMPLEMENTATION

#include <stdexcept>

#include "stb_image.h"

namespace Flint {

ImageTexture::ImageTexture() {
    type = TextureType::IMAGE;
}

ImageTexture::~ImageTexture() {
    if (!resource_ownership) return;

    auto device = Platform::getSingleton()->device;

    vkDestroySampler(device, sampler, nullptr);

    // Should be right before destroying the image itself.
    vkDestroyImageView(device, imageView, nullptr);

    // Destroy handle.
    vkDestroyImage(device, image, nullptr);

    // Release device memory.
    vkFreeMemory(device, imageMemory, nullptr);
}

void ImageTexture::create_image_from_bytes(void *pixels, uint32_t tex_width, uint32_t tex_height, VkFormat tex_format) {
    width = tex_width;
    height = tex_height;

    // Data size per pixel.
    int pixel_bytes;
    if (tex_format == VK_FORMAT_R8G8B8A8_UNORM) {
        pixel_bytes = 4;
    } else if (tex_format == VK_FORMAT_R32G32B32A32_SFLOAT) {
        pixel_bytes = 16;
    } else {
        abort();
    }

    // In bytes.
    VkDeviceSize imageSize = width * height * pixel_bytes;

    // Temporary buffer and device memory.
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    RenderServer::getSingleton()->createBuffer(
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory);

    // Copy the pixel values that we got from the image loading library to the buffer.
    RenderServer::getSingleton()->copyDataToMemory(pixels, stagingBufferMemory, imageSize);

    RenderServer::getSingleton()->createImage(
        width,
        height,
        tex_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        image,
        imageMemory);

    // Transition the texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
    RenderServer::getSingleton()->transitionImageLayout(
        image, tex_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Execute the buffer to image copy operation.
    RenderServer::getSingleton()->copyBufferToImage(
        stagingBuffer, image, 0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    // To be able to start sampling from the texture image in the shader, we need one last transition to prepare it for
    // shader access.
    RenderServer::getSingleton()->transitionImageLayout(
        image, tex_format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Clean up staging stuff.
    vkDestroyBuffer(Platform::getSingleton()->device, stagingBuffer, nullptr);
    vkFreeMemory(Platform::getSingleton()->device, stagingBufferMemory, nullptr);
}

std::shared_ptr<ImageTexture> ImageTexture::from_empty(uint32_t p_width, uint32_t p_height, VkFormat tex_format) {
    assert(p_width != 0 && p_height != 0 && "Creating texture with zero size.");

    auto texture = std::make_shared<ImageTexture>();
    texture->width = p_width;
    texture->height = p_height;

    // Pixel data.
    std::vector<unsigned char> i8_pixels(p_width * p_height * 4, 0);
    std::vector<float> f32_pixels(p_width * p_height * 4, 0);

    void *pixel_data{};
    if (tex_format == VK_FORMAT_R8G8B8A8_UNORM) {
        pixel_data = i8_pixels.data();
    } else if (tex_format == VK_FORMAT_R32G32B32A32_SFLOAT) {
        pixel_data = f32_pixels.data();
    } else {
        abort();
    }

    // Create image and image memory.
    texture->create_image_from_bytes(pixel_data, p_width, p_height, tex_format);

    // Create image view.
    texture->imageView =
        RenderServer::getSingleton()->createImageView(texture->image, tex_format, VK_IMAGE_ASPECT_COLOR_BIT);

    // Create sampler.
    RenderServer::getSingleton()->createTextureSampler(texture->sampler, VK_FILTER_LINEAR);

    return texture;
}

void ImageTexture::set_filter(VkFilter filter) {
    // Destroy the old sampler.
    auto device = Platform::getSingleton()->device;
    vkDestroySampler(device, sampler, nullptr);

    // Create a new sampler.
    RenderServer::getSingleton()->createTextureSampler(sampler, filter);
}

ImageTexture::ImageTexture(const std::string &path) : Texture(path) {
    type = TextureType::IMAGE;

    // The STBI_rgb_alpha value forces the image to be loaded with an alpha channel,
    // even if it doesn't have one, which is nice for consistency with other textures in the future.
    int tex_width, tex_height, tex_channels;
    stbi_uc *pixels = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

    if (!pixels) {
        Flint::Logger::warn("Failed to load image file " + path, "Texture");
        // return Texture::from_empty(4, 4);
        throw std::runtime_error("Failed to load texture image!");
    }

    // Create image and image memory.
    create_image_from_bytes(pixels, tex_width, tex_height, VK_FORMAT_R8G8B8A8_UNORM);

    // Clean up the original pixel array.
    stbi_image_free(pixels);

    // Create image view.
    imageView =
        RenderServer::getSingleton()->createImageView(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    // Create sampler.
    RenderServer::getSingleton()->createTextureSampler(sampler, VK_FILTER_LINEAR);
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

} // namespace Flint
