#ifndef FLINT_MODEL_RENDER_SYSTEM_H
#define FLINT_MODEL_RENDER_SYSTEM_H

#include "../coordinator.h"
#include "../entity.h"
#include "../system.h"

#define GLFW_INCLUDE_VULKAN

#include <set>

#include "GLFW/glfw3.h"

namespace Flint {
class ModelRenderSystem : public System {
public:
    void update(const std::weak_ptr<Coordinator> &p_coordinator);

    void draw(const std::weak_ptr<Coordinator> &p_coordinator, VkCommandBuffer command_buffer);
};
} // namespace Flint

#endif // FLINT_MODEL_RENDER_SYSTEM_H
