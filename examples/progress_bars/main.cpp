#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

class MyProgressBar : public ProgressBar {
    void custom_update(double dt) override {
        float new_value = value + CoreServer::get_singleton()->get_delta() * 10.0f;
        if (new_value > max_value) {
            new_value -= max_value;
        }
        set_value(new_value);
    }
};

int main() {
    App app(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node = std::make_shared<Node>();
        app.tree->get_root()->add_child(node);

        auto node_gui = std::make_shared<Control>();
        node_gui->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        node->add_child(node_gui);

        auto vector_layer = std::make_shared<TextureRect>();
        vector_layer->name = "vector_layer";
        vector_layer->set_size({WINDOW_WIDTH, WINDOW_HEIGHT});
        vector_layer->set_texture(VectorServer::get_singleton()->get_texture());
        vector_layer->set_mouse_filter(MouseFilter::Ignore);
        node->add_child(vector_layer);

        auto vstack_container = std::make_shared<VStackContainer>();
        vstack_container->set_debug_mode(true);
        node->add_child(vstack_container);

        for (int i = 0; i < 5; i++) {
            auto progress_bar = std::make_shared<MyProgressBar>();
            progress_bar->set_value(i * 20);
            vstack_container->add_child(progress_bar);
        }

        vstack_container->set_size({400, 300});
    }

    app.main_loop();

    app.cleanup();

    return EXIT_SUCCESS;
}
