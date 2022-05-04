#ifndef FLINT_NODE_TREE_H
#define FLINT_NODE_TREE_H

#include "control.h"
#include "../../../resources/style_box.h"
#include "../../../resources/font.h"
#include "../../../common/math/rect.h"

#include <cstdint>
#include <memory>

namespace Flint {
    class TreeItem {
    public:
        std::string text;

    };

    class Tree : public Control {
    public:
        Tree();

        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

    private:
        std::optional<StyleBox> theme_panel;
    };
}

#endif //FLINT_NODE_TREE_H
