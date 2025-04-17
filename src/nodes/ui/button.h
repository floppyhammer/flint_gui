#pragma once

#include <functional>

#include "../../resources/style_box.h"
#include "container/box_container.h"
#include "container/margin_container.h"
#include "label.h"
#include "node_ui.h"
#include "texture_rect.h"

namespace revector {

class Button : public NodeUi {
    friend class ButtonGroup;

public:
    Button();

    void input(InputEvent &event) override;

    void update(double dt) override;

    void draw() override;

    void set_position(Vec2F _position) override;

    void set_size(Vec2F _size) override;

    void calc_minimum_size() override;

    void connect_signal(const std::string &signal, const AnyCallable<void> &callback) override;

    void set_text(const std::string &text);

    std::string get_text() const;

    void set_icon_normal(const std::shared_ptr<Image> &icon);

    void set_icon_pressed(const std::shared_ptr<Image> &icon);

    void set_flat(bool flat) {
        flat_ = flat;
    }

    /// The icon will expand until it's height matches that of the button.
    void set_icon_expand(bool enable);

    void set_toggle_mode(bool enable);

    bool get_pressed() const {
        return pressed;
    }

    void set_disabled(bool disabled) {
        disabled_ = disabled;
    }

    void press();

    // Styles.
    StyleBox theme_normal;
    StyleBox theme_hovered;
    StyleBox theme_pressed;
    StyleBox theme_disabled;

protected:
    /**
     * Pressed == ture && hovered == true: Normal buttons
     * Pressed == ture && hovered == false: Toggled buttons
     */
    bool pressed = false;
    bool hovered = false;

    bool pressed_inside = false;

    bool toggle_mode = false;

    bool icon_expand_ = false;

    bool flat_ = false;

    bool disabled_ = false;

    /// Button[HBoxContainer[TextureRect, Label]]
    std::shared_ptr<MarginContainer> margin_container;
    std::shared_ptr<HBoxContainer> hbox_container;
    std::shared_ptr<TextureRect> icon_rect;
    std::shared_ptr<Label> label;

    std::shared_ptr<Image> icon_normal_;
    std::shared_ptr<Image> icon_pressed_;

    // Callbacks.
    std::vector<AnyCallable<void>> pressed_callbacks;
    std::vector<AnyCallable<void>> toggled_callbacks;
    std::vector<AnyCallable<void>> hovered_callbacks;
    std::vector<AnyCallable<void>> down_callbacks;
    std::vector<AnyCallable<void>> up_callbacks;

    void when_pressed();

    void when_toggled(bool pressed);
};

class ButtonGroup {
public:
    void add_button(const std::weak_ptr<Button> &new_button);

    void update();

    std::vector<std::weak_ptr<Button>> buttons;
    std::weak_ptr<Button> pressed_button;

    std::vector<AnyCallable<void>> pressed_callbacks;
};

} // namespace revector
