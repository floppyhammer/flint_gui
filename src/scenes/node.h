#ifndef FLINT_NODE_H
#define FLINT_NODE_H

#include "sub_viewport.h"

#include <vector>
#include <memory>

namespace Flint {
    class Node {
    protected:
        std::vector<Node> children;

        std::shared_ptr<Node> parent;

        void update();

        void draw();

    public:
        virtual void self_update();

        virtual void self_draw();

        /**
         * Get the viewport this node belongs to.
         * @return A pointer to the viewport.
         */
        virtual std::shared_ptr<SubViewport> get_viewport();

        std::shared_ptr<Node> get_parent();

        std::shared_ptr<Node> get_root();
    };
}

#endif //FLINT_NODE_H
