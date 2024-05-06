#include <iostream>
#include <random>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 640;
const uint32_t WINDOW_HEIGHT = 480;

class MyNode : public Node {
    void custom_ready() override {
        auto vbox_container = std::make_shared<VBoxContainer>();
        add_child(vbox_container);
        vbox_container->set_anchor_flag(AnchorFlag::Center);
        vbox_container->set_separation(8);

        {
            auto label = std::make_shared<Label>();
            label->set_custom_minimum_size({256, 64});
            label->set_horizontal_alignment(Alignment::Begin);
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_custom_minimum_size({256, 64});
            label->set_horizontal_alignment(Alignment::Center);
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_custom_minimum_size({256, 64});
            label->set_horizontal_alignment(Alignment::End);
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_vertical_alignment(Alignment::Begin);
            label->set_custom_minimum_size({256, 64});
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_vertical_alignment(Alignment::Center);
            label->set_custom_minimum_size({256, 64});
            vbox_container->add_child(label);
        }

        {
            auto label = std::make_shared<Label>();
            label->set_vertical_alignment(Alignment::End);
            label->set_custom_minimum_size({256, 64});
            vbox_container->add_child(label);
        }
    }
};

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
