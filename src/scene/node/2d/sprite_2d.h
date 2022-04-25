#ifndef FLINT_SPRITE_2D_H
#define FLINT_SPRITE_2D_H

#include "node_2d.h"
#include "../../../resources/mesh.h"
#include "../../../render/mvp_buffer.h"

namespace Flint {
    class Sprite2d : public Node2d {
    public:
        Sprite2d();

        void _update(double delta) override;

        void _draw(VkCommandBuffer p_command_buffer) override;

        void set_texture(std::shared_ptr<Texture> p_texture);

        [[nodiscard]] std::shared_ptr<Texture> get_texture() const;

        void set_mesh(const std::shared_ptr<Mesh2d>& p_mesh);

        void set_material(const std::shared_ptr<Material2d>& p_material);

    private:
        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        void update_mvp();

        Mesh2dPushConstant push_constant;
    };
}

#endif //FLINT_SPRITE_2D_H