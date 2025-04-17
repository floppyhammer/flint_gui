#pragma once

#include <pathfinder/prelude.h>
#include "../render/blit.h"

namespace revector {

class RenderServer {
public:
    static RenderServer *get_singleton() {
        static RenderServer singleton;
        return &singleton;
    }

    void destroy() {
        blit_.reset();
        queue_.reset();
        device_.reset();
        window_builder_.reset();
    }

    std::shared_ptr<Pathfinder::WindowBuilder> window_builder_;
    std::shared_ptr<Pathfinder::Device> device_;
    std::shared_ptr<Pathfinder::Queue> queue_;

    std::shared_ptr<Blit> blit_;
};

} // namespace revector
