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

        void set_margin(const Rect<float> &p_margin);

        void set_margin_all(float value);

    private:
        Rect<float> margin = {8, 8, 8, 8};
    };
}

#endif //FLINT_MARGIN_CONTAINER_H
