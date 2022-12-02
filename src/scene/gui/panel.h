#ifndef FLINT_NODE_PANEL_H
#define FLINT_NODE_PANEL_H

#include <cstdint>
#include <memory>

#include "../../common/geometry.h"
#include "../../resources/font.h"
#include "../../resources/style_box.h"
#include "../../resources/vector_texture.h"
#include "button.h"
#include "control.h"
#include "label.h"

namespace Flint {

/**
 * With different features enabled, a panel can become a window, a collapsable panel, etc.
 * TitleBar[HStackContainer[Button, Label, Button]]
 */
class Panel : public Control {
public:
    Panel();

    void input(InputEvent &event) override;

    void update(double dt) override;

    void draw(VkCommandBuffer p_command_buffer) override;

    void enable_title_bar(bool enabled);

    void propagate_input(InputEvent &event) override;

    void propagate_draw(VkCommandBuffer p_command_buffer) override;

    void set_size(Vec2F p_size) override;

    /// Set title text.
    void set_title(const std::string &title);

private:
    bool title_bar = false;
    bool closable = false;
    bool collapsible = false;
    bool resizable = false;
    bool shrink_title_bar_when_collapsed = false;

    bool collapsed = false;

    float expanded_width = 0;

    float title_bar_height = 48;

    bool title_bar_pressed = false;
    Vec2F title_bar_pressed_mouse_position;
    Vec2F title_bar_pressed_position;

    std::shared_ptr<Label> title_label;
    std::shared_ptr<Button> collapse_button, close_button;
    std::shared_ptr<HStackContainer> title_container;
    std::shared_ptr<VectorTexture> collapsed_tex, expanded_tex, close_tex;

    std::optional<StyleBox> theme_panel;
    std::optional<StyleLine> theme_title_bar_line;
};

} // namespace Flint

#endif // FLINT_NODE_PANEL_H
