#ifndef FLINT_HBOX_CONTAINER_H
#define FLINT_HBOX_CONTAINER_H

#include "container.h"

namespace Flint {
    class HBoxContainer : public Container {
    public:
        HBoxContainer() {
            type = NodeType::HBoxContainer;
        }

        void update(double dt) override;

        void adjust_layout() override;

        Vec2<float> calculate_minimum_size() override;

    protected:
        float separation = 8;
    };
}

#endif //FLINT_HBOX_CONTAINER_H
