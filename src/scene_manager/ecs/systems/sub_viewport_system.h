//
// Created by tannh on 4/4/2022.
//

#ifndef FLINT_SUB_VIEWPORT_SYSTEM_H
#define FLINT_SUB_VIEWPORT_SYSTEM_H

#include "../system.h"
#include "../entity.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <set>

namespace Flint {
    class SubViewportInputSystem : public System {
    public:
        void update();
    };

    class SubViewportOutSystem : public System {
    public:
        void update();
    };
}

#endif //FLINT_SUB_VIEWPORT_SYSTEM_H
