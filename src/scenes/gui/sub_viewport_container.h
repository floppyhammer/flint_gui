#ifndef FLINT_SUB_VIEWPORT_CONTAINER_H
#define FLINT_SUB_VIEWPORT_CONTAINER_H

#include "control.h"

namespace Flint {
    class SubViewportContainer : public Control {
    public:
        std::shared_ptr<SubViewport> viewport;

        /// Intercept viewport searching upwards the scene tree.
        std::shared_ptr<SubViewport> get_viewport() override;
    };
}

#endif //FLINT_SUB_VIEWPORT_CONTAINER_H
