#include <map>

#include "style_box.h"

namespace Flint {

struct Theme {
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
