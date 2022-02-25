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
    }

    void TextureRect::update(double delta) {
    }
}
