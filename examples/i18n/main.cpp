#include <iostream>
#include <random>
#include <stdexcept>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 640;
const uint32_t WINDOW_HEIGHT = 480;

int main() {
    App app(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto margin_container = std::make_shared<MarginContainer>();
        margin_container->set_position({0, 0});
        margin_container->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        margin_container->set_margin_all(32);
        margin_container->set_anchor_flag(AnchorFlag::FullRect);
        app.tree->replace_scene(margin_container);

        auto vstack_container = std::make_shared<VStackContainer>();
        vstack_container->set_separation(16);
        margin_container->add_child(vstack_container);

        auto ll = std::make_shared<Label>("Hello\n你好");
        ll->enable_visual_debug(true);
        vstack_container->add_child(ll);

        auto label = std::make_shared<Label>("مرحبا");
        vstack_container->add_child(label);
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
