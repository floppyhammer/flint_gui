#include <iostream>
#include <random>
#include <stdexcept>

#include "app.h"
#include "nodes/sub_window.h"

using namespace Flint;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto root = app.get_tree()->get_root();

        auto open_window_button = std::make_shared<Button>();
        open_window_button->set_text("Show window");
        auto close_window_button = std::make_shared<Button>();
        close_window_button->set_text("Hide window");
        auto container = std::make_shared<HStackContainer>();
        container->add_child(open_window_button);
        container->add_child(close_window_button);

        root->add_child(container);

        auto sub_window = std::make_shared<SubWindow>(Vec2I{400, 300});
        root->add_child(sub_window);

        auto label = std::make_shared<Label>();
        label->set_text("This is a sub-window.");
        sub_window->add_child(label);

        auto callback1 = [sub_window] { sub_window->set_visibility(true); };
        open_window_button->connect_signal("pressed", callback1);

        auto callback2 = [sub_window] { sub_window->set_visibility(false); };
        close_window_button->connect_signal("pressed", callback2);
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
