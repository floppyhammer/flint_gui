#include <iostream>
#include <random>
#include <stdexcept>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node_ui = std::make_shared<NodeUi>();
        app.get_tree()->replace_scene(node_ui);

        auto vstack_container = std::make_shared<VStackContainer>();
        vstack_container->set_separation(8);
        vstack_container->enable_visual_debug(true);
        vstack_container->set_position({100, 100});
        node_ui->add_child(vstack_container);

        // auto button = std::make_shared<Button>();
        // button->set_text("button");
        // button->container_sizing.flag_h = ContainerSizingFlag::Fill;
        // vstack_container->add_child(button);

        auto check_button = std::make_shared<CheckButton>();
        check_button->container_sizing.flag_h = ContainerSizingFlag::Fill;
        vstack_container->add_child(check_button);

        vstack_container->set_size({800, 100});
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
