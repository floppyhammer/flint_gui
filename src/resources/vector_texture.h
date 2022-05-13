#ifndef FLINT_VECTOR_TEXTURE_H
#define FLINT_VECTOR_TEXTURE_H

#include "texture.h"
#include "../common/color.h"
#include "../common/math/vec2.h"

#include <pathfinder.h>

#include <memory>

namespace Flint {
    struct SvgShape {
        Pathfinder::Shape shape;
        ColorU fill_color = ColorU();
        ColorU stroke_color = ColorU();
        float stroke_width = 0;
        float opacity = 1;
    };

    class VectorTexture : public Texture {
    public:
        VectorTexture();

        /// Create vector texture from a svg file.
        explicit VectorTexture(const std::string &path);

        ~VectorTexture() override;

        /// Create an empty texture with specific size.
        static std::shared_ptr<VectorTexture> from_empty(uint32_t p_width, uint32_t p_height);

        void add_to_canvas(const Vec2<float> &position, const std::shared_ptr<Pathfinder::Canvas> &canvas);

        void add_svg_shape(SvgShape svg_shape);

    protected:
        std::vector<SvgShape> svg_shapes;
    };
}

#endif //FLINT_VECTOR_TEXTURE_H
