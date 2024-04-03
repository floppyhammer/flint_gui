#ifndef FLINT_CONTAINER_H
#define FLINT_CONTAINER_H

#include "../node_ui.h"

namespace Flint {

/**
 * Containers adjust their container children's layouts automatically.
 * This base class is not meant for direct use as a node.
 */
class Container : public NodeUi {
public:
    void update(double dt) override;

    /// Calculates the minimum size of this node, considering all its children' sizing effect.
    void calc_minimum_size() override;

protected:
    /// Hide the constructor as this class is not meant for direct use as a node.
    Container();

    /// The most important method for containers. Adjusts its own size (but not position),
    /// adjusts its children's sizes and local positions.
    virtual void adjust_layout();
};

} // namespace Flint

#endif // FLINT_CONTAINER_H
