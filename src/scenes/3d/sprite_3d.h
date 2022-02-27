//
// Created by tannh on 2/27/2022.
//

#ifndef FLINT_SPRITE_3D_H
#define FLINT_SPRITE_3D_H

#include "node_3d.h"
#include "../../rendering/texture.h"
#include "../../rendering/mesh.h"

namespace Flint {
    class Sprite3D : public Node3D {
        Sprite3D();

    protected:
        void draw() override;
        void update(double delta) override;

        std::shared_ptr<Texture> texture;

        void createDescriptorPool();

        void createDescriptorSets();

        void updateDescriptorSets();

        void set_texture(std::shared_ptr<Texture> p_texture);
    };
}

#endif //FLINT_SPRITE_3D_H
