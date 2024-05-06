#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

class MyNode : public Node {
    void custom_ready() override {
        auto panel = std::make_shared<Panel>();
        panel->set_position({200, 200});
        panel->set_size({400, 300});
        add_child(panel);

        auto scroll_container = std::make_shared<ScrollContainer>();
        scroll_container->set_anchor_flag(AnchorFlag::FullRect);

        panel->add_child(scroll_container);

        auto vbox_container = std::make_shared<VBoxContainer>();
        vbox_container->set_separation(8);
        vbox_container->set_custom_minimum_size({500, 500});
        scroll_container->add_child(vbox_container);

        for (int i = 0; i < 20; i++) {
            auto button = std::make_shared<Button>();
            button->container_sizing.flag_h = ContainerSizingFlag::Fill;
            vbox_container->add_child(button);
        }
    }
};

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
