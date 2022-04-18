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

#include "resources/mesh.h"
#include "resources/texture.h"
#include "core/scene_tree.h"

#include "scene/ecs/entity.h"
#include "scene/ecs/systems/sprite_render_system.h"
#include "scene/ecs/systems/model_render_system.h"
#include "scene/ecs/systems/physics_system.h"

class App {
public:
    void run();

private:
    Flint::SceneTree tree;

    std::queue<Flint::InputEvent> input_queue;

    std::vector<Flint::Entity> entities;

    std::shared_ptr<Flint::Sprite2dRenderSystem> sprite_render_system;

    std::shared_ptr<Flint::ModelRenderSystem> model_render_system;

    std::shared_ptr<Flint::Physics2dSystem> physics_system;

private:
    void main_loop();

    void draw_frame();

    /**
     * Record commands into the current command buffer.
     * @dependency Render pass, swap chain framebuffers, graphics pipeline, vertex buffer, index buffer, pipeline layout.
     */
    void record_commands(std::vector<VkCommandBuffer> &commandBuffers, uint32_t imageIndex) const;
};

#endif //FLINT_APP_H
