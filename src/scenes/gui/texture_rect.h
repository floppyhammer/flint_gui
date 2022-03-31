#ifndef FLINT_TEXTURE_RECT_H
#define FLINT_TEXTURE_RECT_H

#include "control.h"
#include "../../rendering/texture.h"
#include "../../rendering/mesh.h"

#include <memory>

namespace Flint {
    class TextureRect : public Control {
    public:
        TextureRect();

        void set_texture(std::shared_ptr<Texture> p_texture);
        [[nodiscard]] std::shared_ptr<Texture> get_texture() const;

    private:
        void _draw(VkCommandBuffer p_command_buffer) override;

        void _update(double delta) override;

        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        std::shared_ptr<Texture> texture;

        // Create descriptor pool before creating descriptor sets.
        void create_descriptor_pool();

        void create_descriptor_sets();

        // Should call when a new texture is set.
        void update_descriptor_sets();
    };
}

#endif //FLINT_TEXTURE_RECT_H
