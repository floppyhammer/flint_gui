#ifndef FLINT_BUTTON_H
#define FLINT_BUTTON_H

#include "control.h"
#include "label.h"
#include "../../../resources/style_box.h"

namespace Flint {
    class Button : public Control {
    public:
        Button();

        bool pressed = false;
        bool hovered = false;

        void input(std::vector<InputEvent> &input_queue) override;

        void update(double dt) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        void set_position(Vec2<float> p_position) override;

        void set_size(Vec2<float> p_size) override;

        Vec2<float> calculate_minimum_size() override;

    protected:
        StyleBox theme_normal, theme_hovered, theme_pressed;

        std::shared_ptr<Label> label;
    };
}

#endif //FLINT_BUTTON_H
