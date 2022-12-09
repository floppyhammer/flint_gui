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
    App app(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node_ui = std::make_shared<NodeUi>();
        app.tree->replace_scene(node_ui);

        auto hbox_container = std::make_shared<HStackContainer>();
        hbox_container->set_separation(8);
        hbox_container->set_debug_mode(true);
        hbox_container->set_position({100, 100});
        node_ui->add_child(hbox_container);

        for (int _ = 0; _ < 4; _++) {
            auto button = std::make_shared<Button>();
            button->set_text("button");
            hbox_container->add_child(button);

            if (_ == 0) {
                button->container_sizing.expand_h = true;
                button->container_sizing.flag_h = ContainerSizingFlag::ShrinkCenter;

                button->container_sizing.flag_v = ContainerSizingFlag::Fill;
            }
            if (_ == 1) {
                button->container_sizing.flag_v = ContainerSizingFlag::ShrinkStart;
            }
            if (_ == 2) {
                button->container_sizing.flag_v = ContainerSizingFlag::ShrinkCenter;
            }
            if (_ == 3) {
                button->container_sizing.flag_v = ContainerSizingFlag::ShrinkEnd;
            }
        }
        hbox_container->set_size({800, 100});

        auto vbox_container = std::make_shared<VStackContainer>();
        vbox_container->set_separation(8);
        vbox_container->set_position({100, 300});
        vbox_container->set_debug_mode(true);
        node_ui->add_child(vbox_container);

        for (int _ = 0; _ < 4; _++) {
            auto button = std::make_shared<Button>();
            button->set_text("button");
            vbox_container->add_child(button);

            if (_ == 0) {
                button->container_sizing.expand_v = true;
                button->container_sizing.flag_v = ContainerSizingFlag::ShrinkCenter;

                button->container_sizing.flag_h = ContainerSizingFlag::Fill;
            }
            if (_ == 1) {
                button->container_sizing.flag_h = ContainerSizingFlag::ShrinkStart;
            }
            if (_ == 2) {
                button->container_sizing.flag_h = ContainerSizingFlag::ShrinkCenter;
            }
            if (_ == 3) {
                button->container_sizing.flag_h = ContainerSizingFlag::ShrinkEnd;
            }
        }
        vbox_container->set_size({200, 300});
        // ----------------------------------------------------
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
