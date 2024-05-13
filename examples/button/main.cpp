#include <iostream>
#include <random>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

class MyNode : public Node {
    void custom_ready() override {
        auto vbox_container = std::make_shared<VBoxContainer>();
        vbox_container->set_separation(8);
        vbox_container->set_position({100, 100});
        add_child(vbox_container);

        {
            auto button = std::make_shared<Button>();
            button->container_sizing.flag_h = ContainerSizingFlag::ShrinkStart;
            vbox_container->add_child(button);
        }

        {
            auto button = std::make_shared<Button>();
            button->set_icon_normal(ResourceManager::get_singleton()->load<VectorImage>("../assets/icons/Node_Button.svg"));
            button->container_sizing.flag_h = ContainerSizingFlag::ShrinkStart;
            vbox_container->add_child(button);
        }

        {
            auto check_button = std::make_shared<CheckButton>();
            check_button->container_sizing.flag_h = ContainerSizingFlag::ShrinkStart;
            vbox_container->add_child(check_button);
        }

        vbox_container->set_size({800, 100});
    }
};

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
