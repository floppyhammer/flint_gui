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

    void adjust_layout() override;

    void calc_minimum_size();

    void set_margin(const RectF &new_margin);

    void set_margin_all(float value);

private:
    RectF margin = {8, 8, 8, 8};
};

} // namespace Flint

#endif // FLINT_MARGIN_CONTAINER_H
