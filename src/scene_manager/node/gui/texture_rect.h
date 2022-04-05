#ifndef FLINT_TEXTURE_RECT_H
#define FLINT_TEXTURE_RECT_H

#include "node_gui.h"
#include "../../../resources/texture.h"
#include "../../../resources/mesh.h"

#include <memory>

namespace Flint {
    class TextureRect : public NodeGui {
    public:
        TextureRect();

        void set_texture(std::shared_ptr<Texture> p_texture);
        [[nodiscard]] std::shared_ptr<Texture> get_texture() const;

        void _update(double delta) override;

        void _draw(VkCommandBuffer p_command_buffer) override;

    private:
        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;
    };
}

#endif //FLINT_TEXTURE_RECT_H
