#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "node.h"
#include "gui/sub_viewport_container.h"
#include "3d/model.h"
#include "3d/skybox.h"
#include "gui/texture_rect.h"
#include "gui/label.h"
#include "gui/button.h"
#include "gui/stack_container.h"
#include "gui/margin_container.h"
#include "gui/panel.h"
#include "gui/tree.h"
#include "gui/progress_bar.h"
#include "gui/line_edit.h"
#include "sub_viewport.h"
#include "2d/sprite_2d.h"
#include "2d/rigid_body_2d.h"
#include "gui/label.h"
#include "2d/skeleton_2d.h"
#include "gui/spin_box.h"
#include "file_dialog.h"

namespace Flint {
    class SceneTree {
    public:
        SceneTree();

        void input(std::vector<InputEvent> &input_queue) const;

        void update(double dt) const;

        void draw(VkCommandBuffer p_command_buffer) const;

        std::shared_ptr<Node> get_root() const;

    private:
        std::shared_ptr<Node> root;
    };
}

#endif //FLINT_SCENE_TREE_H
