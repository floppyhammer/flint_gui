#pragma once

#include <map>

#include "style_box.h"

namespace Flint {

class Theme {
public:
    Theme() {
        button.colors["text"] = ColorU(229, 229, 229, 255);

        button.colors["text_disabled"] = ColorU(229, 229, 229, 100);

        {
            auto style_box = StyleBox();
            style_box.bg_color = ColorU(32, 32, 32, 255);
            style_box.border_width = 2;
            button.styles["normal"] = style_box;
        }

        {
            auto style_box = StyleBox();
            style_box.bg_color = ColorU(41, 41, 41, 255);
            style_box.border_width = 2;
            button.styles["hovered"] = style_box;
        }

        {
            auto style_box = StyleBox();
            style_box.bg_color = ColorU(45, 45, 45, 255);
            style_box.border_width = 2;
            button.styles["pressed"] = style_box;
        }

        {
            auto style_box = StyleBox();
            style_box.bg_color = ColorU(32, 32, 32, 255);
            style_box.border_width = 2;
            button.styles["disabled"] = style_box;
        }

        label.styles["background"] = StyleBox::from_empty();

        {
            auto style_box = StyleBox();
            style_box.bg_color = ColorU(32, 32, 32, 255);
            style_box.border_color = {75, 75, 75, 100};
            style_box.border_width = 2;
            style_box.corner_radius = 8;
            panel.styles["background"] = style_box;
        }

        {
            {
                auto style_box = StyleBox();
                style_box.bg_color = ColorU(86, 170, 114, 255);
                style_box.border_width = 0;
                style_box.corner_radius = 8;
                collapsing_panel.styles["title_bar"] = style_box;
            }

            {
                auto style_box = StyleBox();
                style_box.bg_color = ColorU(32, 32, 32, 255);
                style_box.border_color = {86, 170, 114, 255};
                style_box.border_width = 2;
                style_box.corner_radius = 8;
                collapsing_panel.styles["background"] = style_box;
            }
        }
    }

    uint32_t font_size = 24;

    struct {
        std::map<std::string, float> constants;
        std::map<std::string, ColorU> colors;
        std::map<std::string, StyleBox> styles;
    } button;

    struct {
        std::map<std::string, float> constants;
        std::map<std::string, ColorU> colors;
        std::map<std::string, StyleBox> styles;
    } label;

    struct {
        std::map<std::string, ColorU> colors;
        std::map<std::string, StyleBox> styles;
    } panel;

    struct {
        std::map<std::string, ColorU> colors;
        std::map<std::string, StyleBox> styles;
    } collapsing_panel;
};

} // namespace Flint
