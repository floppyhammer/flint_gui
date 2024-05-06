#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

class MyNode : public Node {
    void custom_ready() override {
        auto tab_container = std::make_shared<TabContainer>();
        tab_container->set_position({200, 200});
        tab_container->name = "TabContainer";
        tab_container->set_anchor_flag(AnchorFlag::FullRect);
        add_child(tab_container);

        for (int i = 0; i < 3; i++) {
            auto panel = std::make_shared<Panel>();
            auto label = std::make_shared<Label>();
            label->set_text("This is tab " + std::to_string(i));
            tab_container->add_child(label);
        }

        tab_container->set_current_tab(0);
        tab_container->set_size({400, 300});
    }
};

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
