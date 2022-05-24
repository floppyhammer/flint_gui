#ifndef FLINT_VECTOR_SERVER_H
#define FLINT_VECTOR_SERVER_H

#include <pathfinder.h>

namespace Flint {
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

        std::shared_ptr<Pathfinder::Canvas> canvas;
    };
}

#endif //FLINT_VECTOR_SERVER_H
