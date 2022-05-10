#include "panel.h"

#include <string>

namespace Flint {
    Panel::Panel() {
        type = NodeType::Panel;

        theme_panel = std::make_optional(StyleBox());
        theme_panel.value().bg_color = ColorU(50, 50, 50, 255);
        theme_panel.value().shadow_size = 8;
        theme_panel.value().shadow_color = ColorU::black();
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

    void Panel::enable_title_bar(bool enabled) {
        title_bar = enabled;
    }
}
