#ifndef FLINT_NODE_H
#define FLINT_NODE_H

#include "sub_viewport.h"

#include <vector>
#include <memory>

namespace Flint {
    class Node {
    protected:
        //~Node();

        std::vector<Node> children;

        std::shared_ptr<Node> parent;

        void update(double delta);

        void draw();

        virtual void cleanup();

        /// Do this every time something changes in the scene tree.
        virtual void record_commands() = 0;

    public:
        virtual void self_update(double delta);

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
