#pragma once

#include "container.h"

namespace revector {

class BoxContainer : public Container {
public:
    void adjust_layout() override;

    void calc_minimum_size() override;

    void set_separation(float new_separation);

protected:
    BoxContainer() {
        debug_size_box.border_color = ColorU(100, 234, 123);
    }

    /// Separation between UI children.
    float separation = 8;

    /// Direction for organizing UI children.
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

} // namespace revector
