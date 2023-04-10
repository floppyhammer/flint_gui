#include <entt/entt.hpp>
#include <iostream>
#include <random>
#include <stdexcept>

#include "../../src/ecs/graph/graph.h"
#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    Flint::Ecs::run_graph_tests();

    app.main_loop();

    return EXIT_SUCCESS;
}
