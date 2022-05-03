#ifndef FLINT_CONTAINER_H
#define FLINT_CONTAINER_H

#include "control.h"

namespace Flint {
    /**
     * Containers adjust their container children's layouts automatically.
     */
    class Container : public Control {
    public:
        Container() {
            type = NodeType::Container;
        }

        virtual void adjust_layout();
    };
}

#endif //FLINT_CONTAINER_H
