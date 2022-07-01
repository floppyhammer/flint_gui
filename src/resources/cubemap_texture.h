#ifndef FLINT_CUBEMAP_TEXTURE_H
#define FLINT_CUBEMAP_TEXTURE_H

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include "texture.h"
#include "../render/render_server.h"

#include <memory>

namespace Flint {
    class CubemapTexture : public Texture {
    public:
        CubemapTexture(const std::string &path);

        void load_from_file(const std::string &path);

        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

        /// GPU memory.
        VkImage image{};

        /// CPU memory.
        VkDeviceMemory imageMemory{};

        /// Thin wrapper.
        VkImageView imageView{};

        /// How image should be filtered.
        VkSampler sampler{};
    };
}

#endif //FLINT_CUBEMAP_TEXTURE_H
