#ifndef FLINT_BUTTON_H
#define FLINT_BUTTON_H

#include "control.h"
#include "label.h"
#include "texture_rect.h"
#include "hbox_container.h"
#include "../../../resources/style_box.h"

#include <functional>

namespace Flint {
    /**
     * Button[HBoxContainer[TextureRect, Label]]
     */
    class Button : public Control {
    public:
        Button();

        bool pressed = false;
        bool hovered = false;
        bool pressed_inside = false;

        void input(std::vector<InputEvent> &input_queue) override;

        void update(double dt) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        void set_position(Vec2<float> p_position) override;

        void set_size(Vec2<float> p_size) override;

        Vec2<float> calculate_minimum_size() const override;

        void connect_signal(std::string signal, std::function<void()> callback);

        void set_text(const std::string &text);

        void set_icon(const std::shared_ptr<Texture> &p_icon);

    protected:
        std::shared_ptr<HBoxContainer> container;
        std::shared_ptr<TextureRect> icon_rect;
        std::shared_ptr<Label> label;

        std::vector<std::function<void()>> on_pressed_callbacks;
        std::vector<std::function<void()>> on_down_callbacks;
        std::vector<std::function<void()>> on_up_callbacks;
        std::vector<std::function<void()>> on_hovered_callbacks;

        std::optional<StyleBox> theme_normal, theme_hovered, theme_pressed;

    protected:
        void on_pressed();
    };
}

#endif //FLINT_BUTTON_H
