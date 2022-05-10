#ifndef FLINT_NODE_PANEL_H
#define FLINT_NODE_PANEL_H

#include "control.h"
#include "label.h"
#include "../../../resources/style_box.h"
#include "../../../resources/font.h"
#include "../../../common/math/rect.h"

#include <cstdint>
#include <memory>

namespace Flint {
    /**
     * With different features enabled, a panel can become a window, a collapsable panel, etc.
     */
    class Panel : public Control {
    public:
        Panel();

        void update(double dt) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        void enable_title_bar(bool enabled);

    private:
        bool title_bar = false;
        bool closable = false;
        bool collapsible = false;
        bool resizable = false;
        bool shrink_title_bar_when_collapsed = false;

        float title_bar_height = 48;

        std::shared_ptr<Label> title_label;

        std::optional<StyleBox> theme_panel;
    };
}

#endif //FLINT_NODE_PANEL_H
