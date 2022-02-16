#ifndef VULKAN_DEMO_APP_TEXTURE_RECT_H
#define VULKAN_DEMO_APP_TEXTURE_RECT_H

#include "control.h"
#include "../rendering/texture.h"

#include <memory>

namespace SimpleVulkanRenderer {
    class TextureRect : public Control {
    public:
        TextureRect(float viewport_width, float viewport_height);

        ~TextureRect();

        void set_texture(std::shared_ptr<Texture> p_texture);

        std::shared_ptr<Texture> get_texture() const;

        void draw();

    private:
        std::shared_ptr<Texture> texture;

        unsigned int vao = 0;
        unsigned int vbo = 0;
    };
}

#endif //VULKAN_DEMO_APP_TEXTURE_RECT_H
