#include <iostream>
#include <random>
#include <stdexcept>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

class MyNodeUi : public NodeUi {
public:
    std::weak_ptr<PopupMenu> menu;

    virtual void custom_input(InputEvent &event) {
        if (event.type == InputEventType::MouseButton) {
            auto args = event.args.mouse_button;

            if (!args.pressed && args.button == 1) {
                menu.lock()->set_position(args.position);
                menu.lock()->set_visibility(true);
            }

            if (args.pressed && args.button == 0) {
                menu.lock()->set_visibility(false);
            }
        }
    }
};

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Build scene tree. Use a block, so we don't increase ref counts for the node.
    {
        auto root = std::make_shared<MyNodeUi>();
        app.get_tree()->replace_scene(root);

        root->add_child(std::make_shared<Label>());

        auto menu = std::make_shared<PopupMenu>();
        for (int i = 0; i < 4; i++) {
            menu->create_item("Item " + std::to_string(i));
        }
        menu->set_position({400, 400});
        menu->set_visibility(false);
        root->menu = menu;
        root->add_child(menu);
    }

    app.main_loop();

    return EXIT_SUCCESS;
}
