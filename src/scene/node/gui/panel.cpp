#include "panel.h"

#include <string>

namespace Flint {
    Panel::Panel() {
        type = NodeType::Panel;
    }

    void Panel::update(double delta) {
        Control::update(delta);
    }

    void Panel::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton().canvas;

        if (theme_panel.has_value()){
            theme_panel.value().add_to_canvas(position, size, canvas);
        }
    }
}
