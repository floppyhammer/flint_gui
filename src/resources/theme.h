#include <map>

#include "style_box.h"

namespace Flint {

class Theme {
public:
    Theme() {
        button.colors["text"] = ColorU(27, 27, 27, 255);

        {
            auto style_box = StyleBox();
            style_box.bg_color = ColorU(27, 27, 27, 255);
            style_box.border_color = ColorU(100, 100, 100, 255);
            style_box.border_width = 2;
            button.styles["normal"] = style_box;
        }

        {
            auto style_box = StyleBox();
            style_box.border_color = ColorU(163, 163, 163, 255);
            style_box.border_width = 2;
            button.styles["hovered"] = style_box;
        }

        {
            auto style_box = StyleBox();
            style_box.bg_color = ColorU(70, 70, 70, 255);
            style_box.border_color = ColorU(163, 163, 163, 255);
            style_box.border_width = 2;
            button.styles["pressed"] = style_box;
        }

        label.styles["background"] = StyleBox::from_empty();
    }

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
};

} // namespace Flint
