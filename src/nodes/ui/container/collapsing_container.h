#pragma once

#include <optional>

#include "../../../resources/style_box.h"
#include "../button.h"
#include "box_container.h"
#include "../label.h"

namespace Flint {

class CollapsingContainer : public Container {
public:
    CollapsingContainer();

    void update(double dt) override;

    void draw() override;

    void set_theme_panel(StyleBox style_box);

    void calc_minimum_size() override;

    void adjust_layout() override;

private:
    bool collapsed_ = false;

    std::shared_ptr<HBoxContainer> title_bar_container_;
    std::shared_ptr<Label> title_label_;
    std::shared_ptr<Button> collapse_button_;

    float title_bar_height_ = 48;
    float margin_ = 8;

    Vec2F size_before_collapse_;

    std::optional<StyleBox> theme_title_bar_;
    std::optional<StyleBox> theme_panel_;
};

} // namespace Flint
