#ifndef FLINT_VECTOR_SERVER_H
#define FLINT_VECTOR_SERVER_H

#include <pathfinder.h>

#include "../common/geometry.h"

using Pathfinder::Vec2F;
using Pathfinder::Transform2;
using Pathfinder::ColorU;

namespace Flint {
    /// A thin wrapper over Pathfinder shape.
    struct VShape {
        Pathfinder::Shape shape;
        ColorU fill_color = ColorU();
        ColorU stroke_color = ColorU();
        float stroke_width = 0;
        float opacity = 1;
    };

    /**
     * All visible shapes will be collected by the vector server and drawn at once.
     */
    class VectorServer {
    public:
        static VectorServer *get_singleton() {
            static VectorServer singleton;
            return &singleton;
        }

        VectorServer() = default;

        void init(const std::shared_ptr<Pathfinder::Driver> &driver,
                  float p_canvas_width,
                  float p_canvas_height,
                  const std::vector<char> &area_lut_input);

        void draw_line(Vec2F start, Vec2F end, float width, ColorU color);

        void draw_circle(Vec2F center, float radius, float line_width, bool fill, ColorU color);

        void draw_vshape(const VShape &vshape, Transform2 transform);

        std::shared_ptr<Pathfinder::Canvas> canvas;
    };
}

#endif //FLINT_VECTOR_SERVER_H
