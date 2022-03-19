#include "texture_rect.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../core/engine.h"

#include <memory>
#include <chrono>

namespace Flint {

    TextureRect::TextureRect() {
        type = NodeType::TextureRect;

        mesh = std::make_shared<Mesh>(vertices, indices);
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
