#include "node_3d.h"

#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

namespace Flint {
    void Node3D::update_uniform_buffer() {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        // Prepare UBO data.
        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f),
                                time * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                               glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f));

        auto viewport = get_viewport();
        if (viewport != nullptr) {
            ubo.proj = glm::perspective(glm::radians(45.0f),
                                        (float) viewport->size.x / (float) viewport->size.y,
                                        0.1f, 10.0f);

            // GLM was originally designed for OpenGL,
            // where the Y coordinate of the clip coordinates is inverted.
            ubo.proj[1][1] *= -1;

            // Copy the UBO data to the current uniform buffer.
            //RS::getSingleton().copyDataToMemory(&ubo, uniformBuffersMemory[currentImage], sizeof(ubo));
        } else {
            // Do nothing.
        }
    }
}
