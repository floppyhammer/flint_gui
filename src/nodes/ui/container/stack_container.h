#ifndef FLINT_STACK_CONTAINER_H
#define FLINT_STACK_CONTAINER_H

#include "container.h"

namespace Flint {

/// StackContainer is not meant for direct use as a node.
/// Instead, use HStackContainer/VStackContainer.
class StackContainer : public Container {
public:
    void adjust_layout() override;

    // TODO: merge this into adjust_layout to avoid duplicated calculation.
    void calc_minimum_size() override;

    void set_separation(float new_separation);

protected:
    StackContainer() = default;

    /// Separation between UI children.
    float separation = 8;

    /// Organizing direction for UI children.
    bool horizontal = true;
};

class HStackContainer : public StackContainer {
public:
    HStackContainer() {
        type = NodeType::HStackContainer;
        horizontal = true;
    }
};

class VStackContainer : public StackContainer {
public:
    VStackContainer() {
        type = NodeType::VStackContainer;
        horizontal = false;
    }
};

} // namespace Flint

#endif // FLINT_STACK_CONTAINER_H
