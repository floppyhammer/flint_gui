#ifndef FLINT_NODE_PANEL_H
#define FLINT_NODE_PANEL_H

#include <cstdint>
#include <memory>

#include "../../common/geometry.h"
#include "../../resources/font.h"
#include "../../resources/style_box.h"
#include "../../resources/vector_texture.h"
#include "button.h"
#include "label.h"
#include "node_ui.h"

namespace Flint {

/**
 * With different features enabled, a panel can become a window, a collapsable panel, etc.
 * TitleBar[HStackContainer[Button, Label, Button]]
 */
class Panel : public NodeUi {
public:
    Panel();

    void draw() override;

private:
    std::optional<StyleBox> theme_panel;
};

} // namespace Flint

#endif // FLINT_NODE_PANEL_H
