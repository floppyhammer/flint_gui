#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

class MyProgressBar : public ProgressBar {
    void custom_update(double dt) override {
        float new_value = value + Engine::get_singleton()->get_delta() * 10.0f;
        if (new_value > max_value) {
            new_value -= max_value;
        }
        set_value(new_value);
    }
};

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto root = app.get_tree_root();

        auto vbox_container = std::make_shared<VBoxContainer>();
        root->add_child(vbox_container);

        for (int i = 0; i < 5; i++) {
            auto progress_bar = std::make_shared<MyProgressBar>();
            progress_bar->set_value(i * 20);
            vbox_container->add_child(progress_bar);
        }

        vbox_container->set_size({400, 300});
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
