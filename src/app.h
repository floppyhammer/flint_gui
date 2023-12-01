#ifndef FLINT_APP_H
#define FLINT_APP_H

#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <vector>

#include "nodes/scene_tree.h"
#include "resources/image_texture.h"
#include "resources/surface.h"

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
