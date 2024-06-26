#ifndef FLINT_BOX_CONTAINER_H
#define FLINT_BOX_CONTAINER_H

#include "container.h"

namespace Flint {

/// BoxContainer is not meant for direct use as a node.
/// Instead, use HBoxContainer/VBoxContainer.
class BoxContainer : public Container {
public:
    void adjust_layout() override;

    // TODO: merge this into adjust_layout to avoid duplicated calculation.
    void calc_minimum_size() override;

    void set_separation(float new_separation);

protected:
    BoxContainer() {
        debug_size_box.border_color = ColorU(100, 234, 123);
    }

    /// Separation between UI children.
    float separation = 8;

    /// Organizing direction for UI children.
    bool horizontal = true;
};

class HBoxContainer : public BoxContainer {
public:
    HBoxContainer() {
        type = NodeType::HBoxContainer;
        horizontal = true;
    }
};

class VBoxContainer : public BoxContainer {
public:
    VBoxContainer() {
        type = NodeType::VBoxContainer;
        horizontal = false;
    }
};

} // namespace Flint

#endif // FLINT_BOX_CONTAINER_H
