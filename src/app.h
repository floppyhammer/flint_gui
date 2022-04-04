#ifndef FLINT_APP_H
#define FLINT_APP_H

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include "glm/glm.hpp"

#include <vector>
#include <optional>
#include <iostream>
#include <fstream>
#include <array>
#include <cstring>
#include <queue>

#include "rendering/mesh.h"
#include "rendering/texture.h"
#include "core/scene_tree.h"

#include "scene_manager/ecs/entity.h"
#include "scene_manager/ecs/systems/sprite_render_system.h"
#include "scene_manager/ecs/systems/physics_system.h"

class App {
public:
    void run();

    Flint::SceneTree tree;

    std::queue<Flint::InputEvent> input_queue;

    std::vector<Flint::Entity> entities;

    std::shared_ptr<Flint::SpriteRenderSystem> sprite_render_system;

    std::shared_ptr<Flint::PhysicsSystem> physics_system;

private:
    void mainLoop();

    void drawFrame();

    void bindInput();

    /**
     * Record commands into the current command buffer.
     * @dependency Render pass, swap chain framebuffers, graphics pipeline, vertex buffer, index buffer, pipeline layout.
     */
    void recordCommands(std::vector<VkCommandBuffer> &commandBuffers, uint32_t imageIndex) const;
};

#endif //FLINT_APP_H
