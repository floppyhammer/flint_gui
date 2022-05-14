#ifndef FLINT_BOX_CONTAINER_H
#define FLINT_BOX_CONTAINER_H

#include "container.h"

namespace Flint {
    class BoxContainer : public Container {
    public:
        BoxContainer() {
            type = NodeType::BoxContainer;
        }

        void update(double dt) override;

        void adjust_layout() override;

        Vec2<float> calculate_minimum_size() const override;

        void set_separation(float value);

        void make_vertical();

        void make_horizontal();

    protected:
        float separation = 8;

        bool horizontal = true;
    };
}

#endif //FLINT_BOX_CONTAINER_H
