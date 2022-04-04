#ifndef FLINT_SPRITE_RENDER_SYSTEM_H
#define FLINT_SPRITE_RENDER_SYSTEM_H

#include "../system.h"
#include "../entity.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <set>

namespace Flint {
    class SpriteGuiRenderSystem : public System {
    public:
        void update(double dt);

        void draw(VkCommandBuffer command_buffer);
    };

    class Sprite2dRenderSystem : public System {
    public:
        void update(double dt);

        void draw(VkCommandBuffer command_buffer);
    };

    class Sprite3dRenderSystem : public System {
    public:
        void update(double dt);

        void draw(VkCommandBuffer command_buffer);
    };
}

#endif //FLINT_SPRITE_RENDER_SYSTEM_H
