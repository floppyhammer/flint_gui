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
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto root = app.get_tree()->get_root();

        auto vstack_container = std::make_shared<VStackContainer>();
        root->add_child(vstack_container);
        vstack_container->set_anchor_flag(AnchorFlag::Center);
        vstack_container->set_separation(8);

        {
            auto label = std::make_shared<Label>();
            label->set_minimum_size({256, 64});
            label->set_horizontal_alignment(Alignment::Begin);
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_minimum_size({256, 64});
            label->set_horizontal_alignment(Alignment::Center);
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_minimum_size({256, 64});
            label->set_horizontal_alignment(Alignment::End);
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_vertical_alignment(Alignment::Begin);
            label->set_minimum_size({256, 64});
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_vertical_alignment(Alignment::Center);
            label->set_minimum_size({256, 64});
            vstack_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_vertical_alignment(Alignment::End);
            label->set_minimum_size({256, 64});
            vstack_container->add_child(label);
        }
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
