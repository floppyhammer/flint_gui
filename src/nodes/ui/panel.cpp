#include "panel.h"

#include <string>

#include "../../common/utils.h"

namespace Flint {

Panel::Panel() {
    type = NodeType::Panel;

    theme_panel_ = std::make_optional(StyleBox());
    theme_panel_.value().bg_color = ColorU(27, 27, 27, 255);
    theme_panel_.value().border_color = {75, 75, 75, 100};
    theme_panel_.value().border_width = 3;
    theme_panel_.value().corner_radius = 8;
}

void Panel::set_theme_panel(StyleBox style_box) {
    theme_panel_ = std::make_optional(style_box);
}

void Panel::draw() {
    auto vector_server = VectorServer::get_singleton();

    auto global_position = get_global_position();

    if (theme_panel_.has_value()) {
        vector_server->draw_style_box(theme_panel_.value(), global_position, size);
    }
}

} // namespace Flint
