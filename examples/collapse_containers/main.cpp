#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

class MyNode : public Node {
    void custom_ready() override {
        auto collasping_panel = std::make_shared<CollapseContainer>();
        collasping_panel->set_position({400, 200});
        collasping_panel->set_size({500, 400});
        add_child(collasping_panel);

        auto vbox = std::make_shared<VBoxContainer>();
        collasping_panel->add_child(vbox);

        auto label = std::make_shared<Label>();
        label->set_text("This is a label");
        label->container_sizing.expand_h = true;
        label->container_sizing.flag_h = ContainerSizingFlag::Fill;
        vbox->add_child(label);

        auto collasping_panel2 = std::make_shared<CollapseContainer>();
        collasping_panel2->set_color(ColorU{201, 79, 79});
        vbox->add_child(collasping_panel2);

        auto button = std::make_shared<Button>();
        collasping_panel2->add_child(button);

        auto collasping_panel3 = std::make_shared<CollapseContainer>();
        collasping_panel3->set_color(ColorU{66, 105, 183});
        vbox->add_child(collasping_panel3);
    }
};

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
