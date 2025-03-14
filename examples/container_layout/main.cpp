#include <iostream>
#include <random>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

class MyNode : public Node {
    void custom_ready() override {
        auto hbox_container = std::make_shared<HBoxContainer>();
        hbox_container->set_separation(8);
        hbox_container->set_position({100, 100});
        add_child(hbox_container);

        for (int _ = 0; _ < 4; _++) {
            auto button = std::make_shared<Button>();
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

        auto vbox_container = std::make_shared<VBoxContainer>();
        vbox_container->set_separation(8);
        vbox_container->set_position({100, 300});
        add_child(vbox_container);

        for (int _ = 0; _ < 4; _++) {
            auto button = std::make_shared<Button>();
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

        auto grid_container = std::make_shared<GridContainer>();
        grid_container->set_separation(8);
        grid_container->set_position({600, 400});
        grid_container->set_column_number(2);
        add_child(grid_container);

        for (int _ = 0; _ < 4; _++) {
            auto button = std::make_shared<Button>();
            grid_container->add_child(button);

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
        grid_container->set_size({200, 300});
    }
};

int main() {
    App app({1280, 720});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
