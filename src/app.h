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

#include "resources/surface.h"
#include "resources/image_texture.h"

#include "scene/node/scene_tree.h"
#include "scene/node/gui/label.h"

#include "scene/ecs/entity.h"
#include "scene/ecs/world.h"

class App {
public:
    void run();

private:
    Flint::SceneTree tree;

    std::queue<Flint::InputEvent> input_queue;

    std::unique_ptr<Flint::World> world;

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
