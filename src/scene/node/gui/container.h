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

    void set_size(Vec2F p_size) override;

    void update(double dt) override;

    /**
     * One of the most important methods to containers.
     * Calculate the minimum size of this node, considering all its children' sizing effect.
     */
    Vec2F calculate_minimum_size() const override;

protected:
    /**
     * One of the most important methods to containers.
     * Adjust its own size (but not position), adjust its children' sizes and positions.
     */
    virtual void adjust_layout();
};
} // namespace Flint

#endif // FLINT_CONTAINER_H
