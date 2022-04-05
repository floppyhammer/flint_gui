#ifndef FLINT_MODEL_RENDER_SYSTEM_H
#define FLINT_MODEL_RENDER_SYSTEM_H

#include "../system.h"
#include "../entity.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <set>

namespace Flint {
    class ModelRenderSystem : public System {
    public:
        void update();

        void draw(VkCommandBuffer command_buffer);
    };
}

#endif //FLINT_MODEL_RENDER_SYSTEM_H
