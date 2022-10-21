#ifndef FLINT_MARGIN_CONTAINER_H
#define FLINT_MARGIN_CONTAINER_H

#include "container.h"

using Pathfinder::Rect;

namespace Flint {
class MarginContainer : public Container {
public:
    MarginContainer() {
        type = NodeType::MarginContainer;
    }

    void update(double dt) override;

    void adjust_layout() override;

    Vec2F calculate_minimum_size() const override;

    void set_margin(const RectF &p_margin);

    void set_margin_all(float value);

private:
    RectF margin = {8, 8, 8, 8};
};
} // namespace Flint

#endif // FLINT_MARGIN_CONTAINER_H
