#ifndef FLINT_BUTTON_H
#define FLINT_BUTTON_H

#include "control.h"
#include "label.h"
#include "texture_rect.h"
#include "box_container.h"
#include "../../../resources/style_box.h"

#include <functional>

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

        Vec2<float> calculate_minimum_size() const override;

        void connect_signal(std::string signal, std::function<void()> callback);

        void set_text(const std::string &text);

        void set_icon(const std::shared_ptr<Texture> &p_icon);

        bool expand_icon = false;

    protected:
        bool pressed_inside = false;

        /// Button[HBoxContainer[TextureRect, Label]]
        std::shared_ptr<BoxContainer> container;
        std::shared_ptr<TextureRect> icon_rect;
        std::shared_ptr<Label> label;

        // Callbacks.
        std::vector<std::function<void()>> pressed_callbacks;
        std::vector<std::function<void()>> hovered_callbacks;
        std::vector<std::function<void()>> down_callbacks;
        std::vector<std::function<void()>> up_callbacks;

        // Styles.
        std::optional<StyleBox> theme_normal;
        std::optional<StyleBox> theme_hovered;
        std::optional<StyleBox> theme_pressed;

    protected:
        void when_pressed();
    };
}

#endif //FLINT_BUTTON_H
