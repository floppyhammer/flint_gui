#ifndef FLINT_CONTAINER_H
#define FLINT_CONTAINER_H

#include "control.h"

namespace Flint {
    /**
     * Containers adjust their container children's layouts automatically.
     * This base class is not meant for direct use as a node.
     */
    class Container : public Control {
    public:
        Container();

        void set_size(Vec2<float> p_size) override;

        void update(double dt) override;

        Vec2<float> calculate_minimum_size() const override;

    protected:
        virtual void adjust_layout();
    };
}

#endif //FLINT_CONTAINER_H
