#pragma once

#include <optional>

#include "../../../resources/style_box.h"
#include "../button.h"
#include "../label.h"
#include "box_container.h"

namespace revector {

class CollapseContainer : public Container {
public:
    CollapseContainer();

    void update(double dt) override;

    void draw() override;

    void calc_minimum_size() override;

    void set_title(std::string title);

    void adjust_layout() override;

    void set_color(ColorU color) {
        if (theme_title_bar_.has_value()) {
            theme_title_bar_->bg_color = color;
        }
        if (theme_panel_.has_value()) {
            theme_panel_->border_color = color;
        }
    }

    void set_collapse(bool collapse);

private:
    bool collapsed_ = false;

    std::shared_ptr<Button> collapse_button_;

    float title_bar_height_ = 32;
    float margin_ = 8;

    Vec2F size_before_collapse_;

    std::optional<StyleBox> theme_title_bar_;
    std::optional<StyleBox> theme_panel_;
};

} // namespace revector
