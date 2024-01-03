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
        auto node_ui = std::make_shared<NodeUi>();
        app.get_tree()->replace_scene(node_ui);
        node_ui->set_anchor_flag(AnchorFlag::FullRect);

        auto tab_container = std::make_shared<TabContainer>();
        tab_container->set_position({200, 200});
        tab_container->name = "TabContainer";
        tab_container->set_anchor_flag(AnchorFlag::FullRect);
        node_ui->add_child(tab_container);

        for (int i = 0; i < 3; i++) {
            auto panel = std::make_shared<Panel>();
            auto label = std::make_shared<Label>();
            label->set_text("Label" + std::to_string(i));
            tab_container->add_child(label);
        }

        tab_container->set_current_tab(0);
        tab_container->set_size({400, 300});
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
