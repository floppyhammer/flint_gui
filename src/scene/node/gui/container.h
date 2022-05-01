#ifndef FLINT_CONTAINER_H
#define FLINT_CONTAINER_H

#include "node_gui.h"

namespace Flint {
    /**
     * Containers adjust their container children's layouts automatically.
     */
    class Container : public NodeGui {
    public:
        Container() {
            type = NodeType::Container;
        }

        virtual void adjust_layout() = 0;
    };
}

#endif //FLINT_CONTAINER_H
