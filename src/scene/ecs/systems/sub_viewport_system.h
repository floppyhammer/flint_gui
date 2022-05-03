#ifndef FLINT_SUB_VIEWPORT_SYSTEM_H
#define FLINT_SUB_VIEWPORT_SYSTEM_H

#include "../system.h"
#include "../entity.h"
#include "../coordinator.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <set>

namespace Flint {
    class SubViewportInputSystem : public System {
    public:
        void update(const std::weak_ptr<Coordinator>& p_coordinator);
    };

    class SubViewportOutSystem : public System {
    public:
        void update(const std::weak_ptr<Coordinator>& p_coordinator);
    };
}

#endif //FLINT_SUB_VIEWPORT_SYSTEM_H
