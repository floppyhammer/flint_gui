#ifndef FLINT_MATERIAL_H
#define FLINT_MATERIAL_H

#include "texture.h"

#include <memory>
#include <iostream>

class Material {
    // Material name for debugging reason.
    std::string name;
    std::shared_ptr<Texture> diffuse_texture;
    std::shared_ptr<Texture> normal_texture;
};

#endif //FLINT_MATERIAL_H
