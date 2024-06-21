#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

class MyProgressBar : public ProgressBar {
    void custom_update(double dt) override {
        float new_value = value + Engine::get_singleton()->get_delta() * 10.0f;
        if (new_value > max_value) {
            new_value -= max_value;
        }
        set_value(new_value);
    }
};

class MyNode : public Node {
    void custom_ready() override {
        auto vbox_container = std::make_shared<VBoxContainer>();
        add_child(vbox_container);

        for (int i = 0; i < 5; i++) {
            auto progress_bar = std::make_shared<MyProgressBar>();
            progress_bar->set_value(i * 20);
            vbox_container->add_child(progress_bar);
        }

        vbox_container->set_size({400, 300});
    }
};

int main() {
    App app({640, 480});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
