#include <iostream>
#include <random>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

class MyNodeUi : public NodeUi {
public:
    std::weak_ptr<PopupMenu> menu_;

    void custom_ready() override {
        // add_child(std::make_shared<Label>());

        auto menu = std::make_shared<PopupMenu>();
        for (int i = 0; i < 4; i++) {
            menu->create_item("Item " + std::to_string(i));
        }
        menu->set_position({400, 400});
        menu->set_visibility(false);
        menu_ = menu;
        add_child(menu);
    }

    void custom_input(InputEvent &event) override {
        if (event.type == InputEventType::MouseButton) {
            auto args = event.args.mouse_button;

            if (!args.pressed && args.button == 1) {
                menu_.lock()->set_position(args.position);
                menu_.lock()->set_visibility(true);
            }

            if (args.pressed && args.button == 0) {
                menu_.lock()->set_visibility(false);
            }
        }
    }
};

int main() {
    App app({640, 480});

    app.get_tree()->replace_root(std::make_shared<MyNodeUi>());

    app.main_loop();

    return EXIT_SUCCESS;
}
