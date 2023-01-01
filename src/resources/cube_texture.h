#ifndef FLINT_CUBE_TEXTURE_H
#define FLINT_CUBE_TEXTURE_H

#define GLFW_INCLUDE_VULKAN

#include <memory>

#include "../render/render_server.h"
#include "GLFW/glfw3.h"
#include "texture.h"

namespace Flint {
class CubeTexture : public Texture {
public:
    CubeTexture(const std::string &path);

    void load_from_file(const std::string &path);

    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

    /// Handle.
    VkImage image{};

    /// Device memory.
    VkDeviceMemory imageMemory{};

    /// Thin wrapper.
    VkImageView imageView{};

    /// How image should be filtered.
    VkSampler sampler{};
};
} // namespace Flint

#endif // FLINT_CUBE_TEXTURE_H
