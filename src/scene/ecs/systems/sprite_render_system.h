#ifndef FLINT_SPRITE_RENDER_SYSTEM_H
#define FLINT_SPRITE_RENDER_SYSTEM_H

#include "../system.h"
#include "../entity.h"
#include "../coordinator.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <set>

namespace Flint {
    class SpriteGuiRenderSystem : public System {
    public:
        void update(const std::weak_ptr<Coordinator>& p_coordinator, double dt);

        void draw(const std::weak_ptr<Coordinator>& p_coordinator, VkCommandBuffer command_buffer);
    };

    class Sprite2dRenderSystem : public System {
    public:
        void update(const std::weak_ptr<Coordinator>& p_coordinator, double dt);

        void draw(const std::weak_ptr<Coordinator>& p_coordinator, VkCommandBuffer command_buffer);
    };

    class Sprite3dRenderSystem : public System {
    public:
        void update(const std::weak_ptr<Coordinator>& p_coordinator, double dt);

        void draw(std::weak_ptr<Coordinator> p_coordinator, VkCommandBuffer command_buffer);
    };
}

#endif //FLINT_SPRITE_RENDER_SYSTEM_H
