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
        auto root = app.get_tree()->get_root();

        auto vstack_container = std::make_shared<VStackContainer>();
        vstack_container->set_separation(8);
        vstack_container->set_position({100, 100});
        root->add_child(vstack_container);

        {
            auto button = std::make_shared<Button>();
            button->container_sizing.flag_h = ContainerSizingFlag::ShrinkStart;
            vstack_container->add_child(button);
        }

        {
            auto button = std::make_shared<Button>();
            button->set_icon(ResourceManager::get_singleton()->load<VectorImage>("../assets/icons/Node_Button.svg"));
            button->container_sizing.flag_h = ContainerSizingFlag::ShrinkStart;
            vstack_container->add_child(button);
        }

        {
            auto check_button = std::make_shared<CheckButton>();
            check_button->container_sizing.flag_h = ContainerSizingFlag::ShrinkStart;
            vstack_container->add_child(check_button);
        }

        vstack_container->set_size({800, 100});
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
