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

    void ScrollContainer::set_hscroll(int32_t value) {
        if (children.empty()) return;

        Vec2F max_child_min_size = get_max_child_min_size();

        hscroll = std::min(0.0f, max_child_min_size.x - size.x);
    }

    int32_t ScrollContainer::get_hscroll() {
        return hscroll;
    }

    void ScrollContainer::set_vscroll(int32_t value) {
        if (children.empty()) return;

        Vec2F max_child_min_size = get_max_child_min_size();

        vscroll = std::min(0.0f, max_child_min_size.y - size.y);
    }

    int32_t ScrollContainer::get_vscroll() {
        return vscroll;
    }
}
