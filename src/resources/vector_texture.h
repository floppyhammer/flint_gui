#ifndef FLINT_VECTOR_TEXTURE_H
#define FLINT_VECTOR_TEXTURE_H

#include <pathfinder.h>

#include <memory>

#include "../common/geometry.h"
#include "texture.h"

using Pathfinder::ColorU;
using Pathfinder::Transform2;
using Pathfinder::Vec2;

namespace Flint {

/// A thin wrapper over Pathfinder::Path2d.
struct VectorPath {
    Pathfinder::Path2d path2d;
    ColorU fill_color = ColorU();
    ColorU stroke_color = ColorU();
    float stroke_width = 0;
    float opacity = 1;
};

/// A SVG analogy.
class VectorTexture : public Texture {
public:
    VectorTexture(uint32_t p_width, uint32_t p_height);

    /// Create from a SVG file.
    explicit VectorTexture(const std::string &path);

    /// Create empty with a specific size.
    static std::shared_ptr<VectorTexture> from_empty(uint32_t p_width, uint32_t p_height);

    void add_path(const VectorPath &new_path);

    std::vector<VectorPath> &get_paths();

    shared_ptr<Pathfinder::SvgScene> get_svg_scene();

protected:
    std::vector<VectorPath> paths;

    shared_ptr<Pathfinder::SvgScene> svg_scene;
};

} // namespace Flint

#endif // FLINT_VECTOR_TEXTURE_H
