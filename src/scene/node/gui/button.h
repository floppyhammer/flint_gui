#ifndef FLINT_BUTTON_H
#define FLINT_BUTTON_H

#include <functional>

#include "../../../resources/style_box.h"
#include "control.h"
#include "label.h"
#include "margin_container.h"
#include "stack_container.h"
#include "texture_rect.h"

namespace Flint {

class Button : public Control {
public:
    Button();

    bool pressed = false;
    bool hovered = false;

    void input(InputEvent &event) override;

    void update(double dt) override;

    void draw(VkCommandBuffer _command_buffer) override;

    void set_position(Vec2F _position) override;

    void set_size(Vec2F _size) override;

    Vec2F calculate_minimum_size() const override;

    void connect_signal(std::string signal, const std::function<void()> &callback);

    void set_text(const std::string &text);

    void set_icon(const std::shared_ptr<Texture> &_icon);

    /// The icon will expand until it's height matches that of the button.
    void set_expand_icon(bool enable);

protected:
    bool pressed_inside = false;

    bool expand_icon = false;

    /// Button[HStackContainer[TextureRect, Label]]
    std::shared_ptr<MarginContainer> margin_container;
    std::shared_ptr<HStackContainer> hbox_container;
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
} // namespace Flint

#endif // FLINT_BUTTON_H
