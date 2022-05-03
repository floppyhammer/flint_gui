#ifndef FLINT_MODEL_RENDER_SYSTEM_H
#define FLINT_MODEL_RENDER_SYSTEM_H

#include "../system.h"
#include "../entity.h"
#include "../coordinator.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <set>

namespace Flint {
    class ModelRenderSystem : public System {
    public:
        void update(const std::weak_ptr<Coordinator>& p_coordinator);

        void draw(const std::weak_ptr<Coordinator>& p_coordinator, VkCommandBuffer command_buffer);
    };
}

#endif //FLINT_MODEL_RENDER_SYSTEM_H
