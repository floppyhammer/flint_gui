#include "material.h"

std::shared_ptr<Material3d> Material3d::from_default() {
    auto material = std::make_shared<Material3d>();
    material->diffuse_texture = Texture::from_empty(4, 4);

    return material;
}
