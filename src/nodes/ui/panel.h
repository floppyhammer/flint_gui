#pragma once

#include <optional>

#include "../../resources/style_box.h"
#include "node_ui.h"

namespace revector {

class Panel : public NodeUi {
public:
    Panel();

    void draw() override;

    void set_theme_panel(StyleBox style_box);

protected:
    std::optional<StyleBox> theme_panel_;
};

} // namespace revector
