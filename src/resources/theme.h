#include <map>

#include "style_box.h"

namespace Flint {

class Theme {
public:
    Theme() {
        button.colors["text"] = ColorU(27, 27, 27, 255);

        button.styles["normal"] = StyleBox();
        button.styles["normal"].bg_color = ColorU(27, 27, 27, 255);
        button.styles["normal"].border_color = ColorU(100, 100, 100, 255);
        button.styles["normal"].border_width = 2;

        button.styles["hovered"] = StyleBox();
        button.styles["hovered"].border_color = ColorU(163, 163, 163, 255);
        button.styles["hovered"].border_width = 2;

        button.styles["pressed"] = StyleBox();
        button.styles["pressed"].bg_color = ColorU(70, 70, 70, 255);
        button.styles["pressed"].border_color = ColorU(163, 163, 163, 255);
        button.styles["pressed"].border_width = 2;
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
