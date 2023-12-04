#include "panel.h"

#include <string>

#include "../../common/utils.h"

namespace Flint {

Panel::Panel() {
    type = NodeType::Panel;

    theme_panel = std::make_optional(StyleBox());
    theme_panel.value().bg_color = ColorU(27, 27, 27, 255);
    theme_panel.value().border_color = {75, 75, 75, 100};
    theme_panel.value().border_width = 3;
    theme_panel.value().corner_radius = 8;
}

void Panel::draw() {
    auto vector_server = VectorServer::get_singleton();

    auto global_position = get_global_position();

    if (theme_panel.has_value()) {
                vector_server->draw_style_box(theme_panel.value(), get_global_position(), size);
    }
}

} // namespace Flint
