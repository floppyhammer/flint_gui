#include "scroll_container.h"

namespace Flint {
    void ScrollContainer::adjust_layout() {
        if (children.empty()) return;
    }

    void ScrollContainer::update(double dt) {
        Control::update(dt);
    }

    Vec2<float> ScrollContainer::calculate_minimum_size() const {
        Vec2<float> min_size;

        return min_size.max(minimum_size);
    }
}
