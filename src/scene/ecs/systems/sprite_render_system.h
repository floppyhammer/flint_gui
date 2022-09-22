#ifndef FLINT_SPRITE_RENDER_SYSTEM_H
#define FLINT_SPRITE_RENDER_SYSTEM_H

#include "../coordinator.h"
#include "../entity.h"
#include "../system.h"

#define GLFW_INCLUDE_VULKAN

#include <set>

#include "GLFW/glfw3.h"

namespace Flint {
class SpriteGuiRenderSystem : public System {
public:
    void update(const std::weak_ptr<Coordinator> &p_coordinator, double dt);

    void draw(const std::weak_ptr<Coordinator> &p_coordinator, VkCommandBuffer command_buffer);
};

class Sprite2dRenderSystem : public System {
public:
    void update(const std::weak_ptr<Coordinator> &p_coordinator, double dt);

    void draw(const std::weak_ptr<Coordinator> &p_coordinator, VkCommandBuffer command_buffer);
};

class Sprite3dRenderSystem : public System {
public:
    void update(const std::weak_ptr<Coordinator> &p_coordinator, double dt);

    void draw(std::weak_ptr<Coordinator> p_coordinator, VkCommandBuffer command_buffer);
};
} // namespace Flint

#endif // FLINT_SPRITE_RENDER_SYSTEM_H
