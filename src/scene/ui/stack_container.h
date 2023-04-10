#ifndef FLINT_STACK_CONTAINER_H
#define FLINT_STACK_CONTAINER_H

#include "container.h"

namespace Flint::Scene {
/// StackContainer is not meant for direct use as a node.
/// Instead, use HStackContainer/VStackContainer.
class StackContainer : public Container {
public:
    void adjust_layout() override;

    Vec2F calc_minimum_size() const override;

    void set_separation(float new_separation);

protected:
    /// Separation between managed children.
    float separation = 8;

    /// Child organizing direction.
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
