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
        auto node_gui = std::make_shared<Control>();
        node_gui->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        app.tree->get_root()->add_child(node_gui);

        auto vector_layer = std::make_shared<TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        vector_layer->set_texture(VectorServer::get_singleton()->get_texture());
        vector_layer->set_mouse_filter(MouseFilter::Ignore);
        node_gui->add_child(vector_layer);

        auto scroll_container = std::make_shared<ScrollContainer>();
        scroll_container->set_debug_mode(true);
        scroll_container->set_size({400, 300});
        scroll_container->set_position({200, 200});
        node_gui->add_child(scroll_container);

        auto scroll_content = std::make_shared<Tree>();
        scroll_content->set_size({400, 600});
        scroll_container->add_child(scroll_content);
        // ----------------------------------------------------
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
