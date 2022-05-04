#include "tree.h"

#include <string>

namespace Flint {
    Tree::Tree() {
        type = NodeType::Panel;
    }

    void Tree::update(double delta) {
        Control::update(delta);
    }

    void Tree::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton().canvas;

        if (theme_panel.has_value()){
            theme_panel.value().add_to_canvas(position, size, canvas);
        }
    }
}
