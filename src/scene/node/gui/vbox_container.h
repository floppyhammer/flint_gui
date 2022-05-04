#ifndef FLINT_VBOX_CONTAINER_H
#define FLINT_VBOX_CONTAINER_H

#include "container.h"

namespace Flint {
    class VBoxContainer : public Container {
    public:
        VBoxContainer() {
            type = NodeType::VBoxContainer;
        }

        void update(double dt) override;

        void adjust_layout() override;
    };
}

#endif //FLINT_VBOX_CONTAINER_H
