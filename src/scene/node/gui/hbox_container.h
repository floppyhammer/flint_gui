#ifndef FLINT_HBOX_CONTAINER_H
#define FLINT_HBOX_CONTAINER_H

#include "container.h"

namespace Flint {
    class HBoxContainer : public Container {
    public:
        HBoxContainer() {
            type = NodeType::HBoxContainer;
        }

        void adjust_layout() override;
    };
}

#endif //FLINT_HBOX_CONTAINER_H
