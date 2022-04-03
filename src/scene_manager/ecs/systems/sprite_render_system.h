#ifndef FLINT_SPRITE_RENDER_SYSTEM_H
#define FLINT_SPRITE_RENDER_SYSTEM_H

#include "../system.h"
#include "../entity.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <set>

namespace Flint {
    class SpriteRenderSystem : public System {
    public:
        void update(double dt);

        void draw(VkCommandBuffer command_buffer);
    };
}

#endif //FLINT_SPRITE_RENDER_SYSTEM_H
