#ifndef FLINT_MVP_H
#define FLINT_MVP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <vector>

namespace Flint {

/// MVP, which will be sent to the vertex stage.
/// Shared by 2D and 3D.
struct ModelViewProjection {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    glm::mat4 calculate_mvp() const;
};

// TODO: We might need to push model, view and projection separately.
struct MvpPushConstant {
    glm::mat4 mvp;
};

/// Uniform buffer for MVP. (Push constants are preferred over this.)
class MvpBuffer {
public:
    MvpBuffer();

    ~MvpBuffer();

    /// We have a uniform buffer per swap chain image.
    std::vector<VkBuffer> uniform_buffers;
    std::vector<VkDeviceMemory> uniform_buffers_memory;

    /**
     * Update buffer. Update buffer data simply by memory mapping.
     * @param currentImage Current image, which has different meaning from `current frame`.
     */
    void update_uniform_buffer(ModelViewProjection mvp);

private:
    /**
     * Create buffer for uniform data.
     * @dependency Swap chain count.
     */
    void create_uniform_buffers();

    void free_uniform_buffers();
};

} // namespace Flint

#endif // FLINT_MVP_H
