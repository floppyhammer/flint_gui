#ifndef FLINT_BUTTON_H
#define FLINT_BUTTON_H

#include "control.h"

#include "../../../resources/style_box.h"

namespace Flint {
    class Button : public Control {
    public:
        Button();

        bool pressed = false;
        bool hovered = false;

        void input(std::vector<InputEvent> &input_queue) override;

        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

    protected:
        StyleBox theme_normal, theme_hovered, theme_pressed;
    };
}

#endif //FLINT_BUTTON_H
