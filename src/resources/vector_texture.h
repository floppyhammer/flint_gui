#ifndef FLINT_VECTOR_TEXTURE_H
#define FLINT_VECTOR_TEXTURE_H

#include "texture.h"
#include "../common/geometry.h"
#include "../servers/vector_server.h"

#include <pathfinder.h>

#include <memory>

using Pathfinder::Vec2;
using Pathfinder::ColorU;

namespace Flint {
    class VectorTexture : public Texture {
    public:
        VectorTexture();

        /// Create vector texture from a svg file.
        explicit VectorTexture(const std::string &path);

        ~VectorTexture() override;

        /// Create an empty texture with a specific size.
        static std::shared_ptr<VectorTexture> from_empty(uint32_t p_width, uint32_t p_height);

        /// Draw vector paths.
        void add_to_canvas(const Vec2<float> &position, const std::shared_ptr<Pathfinder::Canvas> &canvas);

        /// Replace vector paths.
        void set_vector_paths(const std::vector<VectorPath> &p_vector_paths);

    protected:
        std::vector<VectorPath> vector_paths;
    };
}

#endif //FLINT_VECTOR_TEXTURE_H
