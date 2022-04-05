#ifndef FLINT_SPRITE_2D_H
#define FLINT_SPRITE_2D_H

#include "node_2d.h"
#include "../../../resources/mesh.h"
#include "../../../rendering/mvp_buffer.h"

namespace Flint {
    class Sprite2d : public Node2d {
    public:
        Sprite2d();

        void _draw(VkCommandBuffer p_command_buffer);

    private:
        std::shared_ptr<Mesh2D> mesh;

        std::shared_ptr<Material2D> material;

        std::shared_ptr<MvpBuffer> mvp_buffer;

        void draw(VkCommandBuffer p_command_buffer);

        void update_mvp();
    };
}

#endif //FLINT_SPRITE_2D_H
