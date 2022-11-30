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
        auto node = std::make_shared<Node>();

        auto node_gui = std::make_shared<Control>();
        node_gui->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        node->add_child(node_gui);

        auto vector_layer = std::make_shared<TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        vector_layer->set_texture(VectorServer::get_singleton()->get_texture());
        vector_layer->set_mouse_filter(MouseFilter::Ignore);

        auto tab_container = std::make_shared<TabContainer>();
        tab_container->set_position({200, 200});
        tab_container->name = "TabContainer";
        node->add_child(tab_container);

        for (int i = 0; i < 3; i++) {
            auto panel = std::make_shared<Panel>();
            tab_container->add_child(std::make_shared<Label>("Label" + std::to_string(i)));
        }

        tab_container->set_current_tab(0);
        tab_container->set_size({400, 300});

        node->add_child(vector_layer);
        app.tree->get_root()->add_child(node);
        // ----------------------------------------------------
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
