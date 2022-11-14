#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "2d/rigid_body_2d.h"
#include "2d/skeleton_2d.h"
#include "2d/sprite_2d.h"
#include "3d/model.h"
#include "3d/skybox.h"
#include "file_dialog.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/margin_container.h"
#include "gui/panel.h"
#include "gui/progress_bar.h"
#include "gui/scroll_container.h"
#include "gui/spin_box.h"
#include "gui/stack_container.h"
#include "gui/sub_viewport_container.h"
#include "gui/text_edit.h"
#include "gui/texture_rect.h"
#include "gui/tree.h"
#include "gui/tab_container.h"
#include "node.h"
#include "sub_viewport.h"

namespace Flint {

class SceneTree {
public:
    SceneTree();

    void input(std::vector<InputEvent> &input_queue) const;

    void update(double dt) const;

    void draw(VkCommandBuffer p_command_buffer) const;

    /// The Root of the scene tree is a Node.
    std::shared_ptr<Node> get_root() const;

private:
    std::shared_ptr<Node> root;
};

} // namespace Flint

#endif // FLINT_SCENE_TREE_H
