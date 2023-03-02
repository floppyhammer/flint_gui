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
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto node_ui = std::make_shared<NodeUi>();
        app.get_tree()->replace_scene(node_ui);

        auto open_window_button = std::make_shared<Button>();
        open_window_button->set_text("Show window");
        auto close_window_button = std::make_shared<Button>();
        close_window_button->set_text("Hide window");
        auto container = std::make_shared<HStackContainer>();
        container->add_child(open_window_button);
        container->add_child(close_window_button);

        node_ui->add_child(container);

        auto another_window = std::make_shared<WindowProxy>(Vec2I{400, 300}, false);
        node_ui->add_child(another_window);

        auto another_ui_layer = std::make_shared<UiLayer>();
        another_ui_layer->add_child(std::make_shared<Label>("This is another window"));
        another_window->add_child(another_ui_layer);

        auto callback1 = [another_window] { another_window->set_visibility(true); };
        open_window_button->connect_signal("pressed", callback1);

        auto callback2 = [another_window] { another_window->set_visibility(false); };
        close_window_button->connect_signal("pressed", callback2);
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
