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

#include "rendering/mesh.h"
#include "rendering/texture.h"
#include "scenes/3d/mesh_instance_3d.h"
#include "core/scene_tree.h"

class App {
public:
    void run();

    Flint::SceneTree tree;

private:
    void mainLoop();

    void drawFrame();

    /**
     * Record commands into the current command buffer.
     * @dependency Render pass, swap chain framebuffers, graphics pipeline, vertex buffer, index buffer, pipeline layout.
     */
    void recordCommands(std::vector<VkCommandBuffer> &commandBuffers, uint32_t imageIndex);
};

#endif //FLINT_APP_H
