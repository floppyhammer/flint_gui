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

        ~TextureRect();

        void set_texture(std::shared_ptr<Texture> p_texture);
        std::shared_ptr<Texture> get_texture() const;

        void draw() override;

        void update(double delta) override;

    private:
        std::shared_ptr<Texture> texture;

        std::shared_ptr<Mesh> mesh;
    };
}

#endif //FLINT_TEXTURE_RECT_H
