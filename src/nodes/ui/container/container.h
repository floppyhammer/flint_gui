#pragma once

#include "../node_ui.h"

namespace revector {

/**
 * Containers adjust their container children's layouts automatically.
 * This base class is not meant for direct use as a node.
 */
class Container : public NodeUi {
public:
    void update(double dt) override;

    /// Calculates the minimum size of this node, considering all its children's sizing effect.
    void calc_minimum_size() override;

    void draw() override;

    void set_theme_bg(StyleBox style_box);

protected:
    /// Hide the constructor as this class is not meant for direct use as a node.
    Container();

    /// The most important method for containers. Adjusts its own size (but not position),
    /// adjusts its children's sizes and local positions.
    virtual void adjust_layout();

    std::vector<NodeUi *> get_visible_ui_children() const;

    std::optional<StyleBox> theme_bg_;
};

} // namespace revector
