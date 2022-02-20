#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "../scenes/node.h"

namespace Flint {
    class SceneTree {
        std::shared_ptr<Node> root;
    };
}

#endif //FLINT_SCENE_TREE_H
