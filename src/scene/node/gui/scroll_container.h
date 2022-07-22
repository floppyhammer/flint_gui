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

    protected:
        bool horizontal_scroll = true;

        bool vertical_scroll = true;
    };
}

#endif //FLINT_SCROLL_CONTAINER_H
