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

        auto image_texture = ResourceManager::get_singleton()->load<ImageTexture>("../assets/duck.png");

        auto texture_rect_image = std::make_shared<TextureRect>();
        texture_rect_image->set_texture(image_texture);
        texture_rect_image->set_debug_mode(true);
        texture_rect_image->set_size({400, 200});
        node_gui->add_child(texture_rect_image);

//        auto texture_rect_svg = std::make_shared<TextureRect>();
//        node_gui->add_child(texture_rect_svg);

        node->add_child(vector_layer);
        app.tree->get_root()->add_child(node);
        // ----------------------------------------------------
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
