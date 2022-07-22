#ifndef FLINT_SCROLL_CONTAINER_H
#define FLINT_SCROLL_CONTAINER_H

#include "container.h"

namespace Flint {
    /// Scroll container should use a separate Pathfinder::Scene and use the ViewBox property
    /// to achieve the clipping effect.
    class ScrollContainer : public Container {
    public:
        ScrollContainer() {
            type = NodeType::ScrollContainer;
        }

        void update(double dt) override;

        void adjust_layout() override;

        Vec2<float> calculate_minimum_size() const override;

        void set_hscroll(int32_t value);

        int32_t get_hscroll();

        void set_vscroll(int32_t value);

        int32_t get_vscroll();

    protected:
        bool horizontal_mode = true;
        bool vertical_mode = true;

        int32_t hscroll = 0;
        int32_t vscroll = 0;
    };
}

#endif //FLINT_SCROLL_CONTAINER_H
