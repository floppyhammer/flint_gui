#include "material.h"

std::shared_ptr<Material3D> Material3D::from_default() {
    auto material = std::make_shared<Material3D>();
    material->diffuse_texture = Texture::from_empty(4, 4);

    return material;
}
