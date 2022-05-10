#ifndef FLINT_MARGIN_CONTAINER_H
#define FLINT_MARGIN_CONTAINER_H

#include "container.h"

namespace Flint {
    class MarginContainer : public Container {
    public:
        MarginContainer() {
            type = NodeType::MarginContainer;
        }

        void update(double dt) override;

        void adjust_layout() override;

    private:
        float margin_left = 8, margin_right = 8, margin_top = 8, margin_bottom = 8;
    };
}

#endif //FLINT_MARGIN_CONTAINER_H
