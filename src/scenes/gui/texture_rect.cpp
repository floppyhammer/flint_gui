#include "texture_rect.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../core/engine.h"

#include <memory>
#include <chrono>

namespace Flint {
    const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    // For index buffer.
    const std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0
    };

    TextureRect::TextureRect() {
        mesh = std::make_shared<Mesh>(vertices, indices);

        // Pipeline should be built on the rendering server, as all texture rects use the same pipeline but with
        // different uniform buffers.


    }

    TextureRect::~TextureRect() {
    }

    void TextureRect::set_texture(std::shared_ptr<Texture> p_texture) {
        texture = std::move(p_texture);
    }

    std::shared_ptr<Texture> TextureRect::get_texture() const {
        return texture;
    }

    void TextureRect::draw() {
        Control::draw();

        // Should get this from RS.
        uint32_t current_image = 0;

        // Copy the data in the uniform buffer object to the current uniform buffer.
        void* data;
        vkMapMemory(RS::getSingleton().device, uniform_buffers_memory[current_image], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(RS::getSingleton().device, uniform_buffers_memory[current_image]);
    }

    void TextureRect::update(double delta) {
        ubo.model = glm::rotate(glm::mat4(1.0f), (float) Engine::getSingleton().get_elapsed() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        auto viewport = get_viewport();
        if (viewport == nullptr) return;

        ubo.proj = glm::perspective(glm::radians(45.0f), (float) viewport->size.x / (float) viewport->size.y, 0.1f, 10.0f);

        // GLM was originally designed for OpenGL,
        // where the Y coordinate of the clip coordinates is inverted.
        ubo.proj[1][1] *= -1;


    }
}
