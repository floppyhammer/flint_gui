#ifndef FLINT_MATERIAL_H
#define FLINT_MATERIAL_H

#include "texture.h"

#include <memory>
#include <iostream>

/// Material Resources

class Material {
public:
    // Material name for debugging reason.
    std::string name;
};

class Material2D : public Material {
public:
    std::shared_ptr<Texture> texture;
};

class Material3D : public Material {
public:
    std::shared_ptr<Texture> diffuse_texture;
    std::shared_ptr<Texture> normal_texture;
};

#endif //FLINT_MATERIAL_H
