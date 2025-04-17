#include <iostream>
#include <random>

#include "app.h"
#include "nodes/sub_window.h"

using namespace revector;

class MyNode : public Node {
    void custom_ready() override {
        auto open_window_button = std::make_shared<Button>();
        open_window_button->set_text("Show window");
        auto close_window_button = std::make_shared<Button>();
        close_window_button->set_text("Hide window");
        auto container = std::make_shared<HBoxContainer>();
        container->add_child(open_window_button);
        container->add_child(close_window_button);

        add_child(container);

        auto sub_window = std::make_shared<SubWindow>(Vec2I{480, 320});
        add_child(sub_window);

        auto label = std::make_shared<Label>();
        label->set_text("This is a sub-window.");
        sub_window->add_child(label);

        auto callback1 = [sub_window] { sub_window->set_visibility(true); };
        open_window_button->connect_signal("pressed", callback1);

        auto callback2 = [sub_window] { sub_window->set_visibility(false); };
        close_window_button->connect_signal("pressed", callback2);
    }
};

int main() {
    App app({640, 480});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
