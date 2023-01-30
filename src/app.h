#ifndef FLINT_APP_H
#define FLINT_APP_H

#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "resources/image_texture.h"
#include "resources/surface.h"
#include "scene/scene_tree.h"

class App {
public:
    App(int32_t window_width, int32_t window_height);

    void main_loop();

    void cleanup();

    std::unique_ptr<Flint::SceneTree> tree;

private:
    /**
     * Record commands into the current command buffer.
     * @dependency Render pass, swap chain framebuffers, graphics pipeline, vertex buffer, index buffer, pipeline
     * layout.
     */
    void record_commands(std::vector<VkCommandBuffer> &command_buffers, uint32_t image_index) const;
};

#endif // FLINT_APP_H
