#ifndef FLINT_NODE_PANEL_H
#define FLINT_NODE_PANEL_H

#include "control.h"
#include "label.h"
#include "button.h"
#include "../../../resources/style_box.h"
#include "../../../resources/vector_texture.h"
#include "../../../resources/font.h"
#include "../../../common/math/rect.h"

#include <cstdint>
#include <memory>

namespace Flint {
    /**
     * With different features enabled, a panel can become a window, a collapsable panel, etc.
     * TitleBar[HBoxContainer[Button, Label, Button]]
     */
    class Panel : public Control {
    public:
        Panel();

        void input(std::vector<InputEvent> &input_queue) override;

        void update(double dt) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        void enable_title_bar(bool enabled);

        void propagate_input(std::vector<InputEvent> &input_queue) override;

        void propagate_draw(VkCommandBuffer p_command_buffer) override;

        void set_size(Vec2<float> p_size) override;

    private:
        bool title_bar = false;
        bool closable = false;
        bool collapsible = false;
        bool resizable = false;
        bool shrink_title_bar_when_collapsed = false;

        bool collapsed = false;

        float title_bar_height = 48;

        bool title_bar_pressed = false;
        Vec2<float> title_bar_pressed_mouse_position;
        Vec2<float> title_bar_pressed_position;

        std::shared_ptr<Label> title_label;
        std::shared_ptr<Button> collapse_button, close_button;
        std::shared_ptr<HBoxContainer> container;
        std::shared_ptr<VectorTexture> collapse_icon, expand_icon;

        std::optional<StyleBox> theme_panel;
    };
}

#endif //FLINT_NODE_PANEL_H
