#include <iostream>
#include <random>
#include <stdexcept>

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

        // Scene panel.
        // ----------------------------------------------------
        auto scene_panel = std::make_shared<Panel>();
        scene_panel->set_position({50, 100});
        scene_panel->set_title("Scene");
        scene_panel->set_size({400, 400});
        auto vector_layer = std::make_shared<TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        vector_layer->set_texture(VectorServer::get_singleton()->get_texture());
        vector_layer->set_mouse_filter(MouseFilter::Ignore);

        auto item_tree = std::make_shared<Tree>();
        item_tree->set_size({400, 400});
        // ----------------------------------------------------

        auto skeleton = std::make_shared<Skeleton2d>();
        skeleton->position = {1000, 300};
        node->add_child(skeleton);

        node->add_child(vector_layer);

        auto margin_container = std::make_shared<MarginContainer>();
        margin_container->set_position({0, 48});
        margin_container->set_size({400, 400});
        margin_container->add_child(item_tree);
        scene_panel->add_child(margin_container);

        node_gui->add_child(scene_panel);
        scene_panel->set_anchor_flag(AnchorFlag::TopLeft);

        app.tree->get_root()->add_child(node);
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
