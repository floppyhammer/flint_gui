#ifndef FLINT_VBOX_CONTAINER_H
#define FLINT_VBOX_CONTAINER_H

#include "container.h"

namespace Flint {
    class VBoxContainer : public Container {
    public:
        VBoxContainer() {
            type = NodeType::VBoxContainer;
        }

        void update(double dt) override;

        void adjust_layout() override;

        Vec2<float> calculate_minimum_size() const override;

    protected:
        float separation = 8;
    };
}

#endif //FLINT_VBOX_CONTAINER_H
