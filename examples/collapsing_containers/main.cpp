#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

class MyNode : public Node {
    void custom_ready() override {
        auto collasping_panel = std::make_shared<CollapsingContainer>();
        collasping_panel->set_position({400, 200});
        collasping_panel->set_size({500, 400});
        add_child(collasping_panel);

        auto vbox = std::make_shared<VBoxContainer>();
        collasping_panel->add_child(vbox);

        auto collasping_panel2 = std::make_shared<CollapsingContainer>();
        vbox->add_child(collasping_panel2);

        auto button = std::make_shared<Button>();
        collasping_panel2->add_child(button);

        auto collasping_panel3 = std::make_shared<CollapsingContainer>();
        vbox->add_child(collasping_panel3);
    }
};

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
