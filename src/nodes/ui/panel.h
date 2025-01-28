#ifndef FLINT_NODE_PANEL_H
#define FLINT_NODE_PANEL_H

#include <optional>

#include "../../resources/style_box.h"
#include "node_ui.h"

namespace Flint {

class Panel : public NodeUi {
public:
    Panel();

    void draw() override;

    void set_theme_panel(StyleBox style_box);

protected:
    std::optional<StyleBox> theme_panel_;
};

} // namespace Flint

#endif // FLINT_NODE_PANEL_H
