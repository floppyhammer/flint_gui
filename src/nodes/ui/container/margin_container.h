#ifndef FLINT_MARGIN_CONTAINER_H
#define FLINT_MARGIN_CONTAINER_H

#include "container.h"

using Pathfinder::Rect;

namespace Flint {

class MarginContainer final : public Container {
public:
    MarginContainer() {
        type = NodeType::MarginContainer;
    }

    void calc_minimum_size() override;

    void adjust_layout() override;

    void set_margin(const RectF &margin);

    void set_margin_all(float margin);

private:
    RectF margin_ = {8, 8, 8, 8};
};

} // namespace Flint

#endif // FLINT_MARGIN_CONTAINER_H
