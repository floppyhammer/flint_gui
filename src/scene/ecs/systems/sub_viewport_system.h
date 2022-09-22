#ifndef FLINT_SUB_VIEWPORT_SYSTEM_H
#define FLINT_SUB_VIEWPORT_SYSTEM_H

#include "../coordinator.h"
#include "../entity.h"
#include "../system.h"

#define GLFW_INCLUDE_VULKAN

#include <set>

#include "GLFW/glfw3.h"

namespace Flint {
class SubViewportInputSystem : public System {
public:
    void update(const std::weak_ptr<Coordinator> &p_coordinator);
};

class SubViewportOutSystem : public System {
public:
    void update(const std::weak_ptr<Coordinator> &p_coordinator);
};
} // namespace Flint

#endif // FLINT_SUB_VIEWPORT_SYSTEM_H
