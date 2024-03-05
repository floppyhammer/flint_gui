#ifndef FLINT_CONTAINER_H
#define FLINT_CONTAINER_H

#include "node_ui.h"

namespace Flint {

/**
 * Containers adjust their container children's layouts automatically.
 * This base class is not meant for direct use as a node.
 */
class Container : public NodeUi {
public:
    void set_size(Vec2F new_size) override;

    void update(double dt) override;

    /**
     * One of the most important methods to containers.
     * Calculate the minimum size of this node, considering all its children' sizing effect.
     */
    Vec2F calc_minimum_size() const override;

protected:
    // This class is not meant for direct use as a node.
    Container();

    /**
     * One of the most important methods to containers.
     * Adjust its own size (but not position), adjust its children' sizes and positions.
     */
    virtual void adjust_layout();
};

} // namespace Flint

#endif // FLINT_CONTAINER_H
