#include "panel.h"

#include <string>

#include "../../common/utils.h"
#include "../../resources/default_resource.h"

namespace Flint {

Panel::Panel() {
    type = NodeType::Panel;

    auto default_theme = DefaultResource::get_singleton()->get_default_theme();

    theme_panel_ = std::make_optional(default_theme->panel.styles["background"]);
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
