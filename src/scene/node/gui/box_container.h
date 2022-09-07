#ifndef FLINT_BOX_CONTAINER_H
#define FLINT_BOX_CONTAINER_H

#include "container.h"

namespace Flint {
    /// BoxContainer is not meant for direct use as a node.
    /// Instead, use HBoxContainer/VBoxContainer.
    class BoxContainer : public Container {
    public:
        void update(double dt) override;

        void adjust_layout() override;

        Vec2<float> calculate_minimum_size() const override;

        void set_separation(float new_separation);

    protected:
        /// Separation between managed children.
        float separation = 8;

        /// Child organizing direction.
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
}

#endif //FLINT_BOX_CONTAINER_H
