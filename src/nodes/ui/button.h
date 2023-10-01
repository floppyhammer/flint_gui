#ifndef FLINT_BUTTON_H
#define FLINT_BUTTON_H

#include <functional>

#include "../../resources/style_box.h"
#include "label.h"
#include "margin_container.h"
#include "node_ui.h"
#include "stack_container.h"
#include "texture_rect.h"

namespace Flint {

class Button : public NodeUi {
public:
    Button();

    /**
     * Pressed == ture && hovered == true: Normal buttons
     * Pressed == ture && hovered == false: Toggled buttons
     */
    bool pressed = false;
    bool hovered = false;

    void input(InputEvent &event) override;

    void update(double dt) override;

    void draw() override;

    void set_position(Vec2F _position) override;

    void set_size(Vec2F _size) override;

    Vec2F calc_minimum_size() const override;

    void connect_signal(const std::string &signal, const std::function<void()> &callback);

    void set_text(const std::string &text);

    void set_icon(const std::shared_ptr<Texture> &_icon);

    /// The icon will expand until it's height matches that of the button.
    void set_expand_icon(bool enable);

    void set_toggle_mode(bool enable);

    // Styles.
    StyleBox theme_normal;
    StyleBox theme_hovered;
    StyleBox theme_pressed;

protected:
    bool pressed_inside = false;

    bool toggle_mode = false;

    bool expand_icon = false;

    /// Button[HStackContainer[TextureRect, Label]]
    std::shared_ptr<MarginContainer> margin_container;
    std::shared_ptr<HStackContainer> hstack_container;
    std::shared_ptr<TextureRect> icon_rect;
    std::shared_ptr<Label> label;

    // Callbacks.
    std::vector<std::function<void()>> pressed_callbacks;
    std::vector<std::function<void()>> hovered_callbacks;
    std::vector<std::function<void()>> down_callbacks;
    std::vector<std::function<void()>> up_callbacks;

protected:
    void when_pressed();
};

class ButtonGroup {
public:
    void add_button(const std::weak_ptr<Button> &new_button);

    void update();

    std::vector<std::weak_ptr<Button>> buttons;
    std::weak_ptr<Button> pressed_button;

    std::vector<std::function<void()>> pressed_callbacks;
};

} // namespace Flint

#endif // FLINT_BUTTON_H
