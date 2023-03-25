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

        auto panel = std::make_shared<Panel>();
        panel->set_position({200, 200});
        panel->set_size({400, 300});
        node_ui->add_child(panel);

        auto scroll_container = std::make_shared<ScrollContainer>();
        scroll_container->set_size({400, 300});
        panel->add_child(scroll_container);

        auto scroll_content = std::make_shared<Tree>();
        scroll_content->set_size({400, 800});
        scroll_container->add_child(scroll_content);
        // ----------------------------------------------------
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
