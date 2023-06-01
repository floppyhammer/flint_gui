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

using namespace Flint;

class App {
public:
    App(Vec2I window_size);

    ~App();

    void main_loop();

    SceneTree* get_tree();

private:
    void cleanup();

private:
    std::unique_ptr<SceneTree> tree;
};

#endif // FLINT_APP_H
