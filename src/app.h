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

#include "scene/ecs/entity.h"
#include "scene/ecs/world.h"

class App {
public:
    void run();

private:
    std::unique_ptr<Flint::SceneTree> tree;

    std::unique_ptr<Flint::World> world;

private:
    void init();

    void main_loop();

    void cleanup();

    /**
     * Record commands into the current command buffer.
     * @dependency Render pass, swap chain framebuffers, graphics pipeline, vertex buffer, index buffer, pipeline layout.
     */
    void record_commands(std::vector<VkCommandBuffer> &commandBuffers, uint32_t imageIndex) const;
};

#endif //FLINT_APP_H
