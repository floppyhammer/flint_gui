#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "file_dialog.h"
#include "node.h"
#include "scene/2d/rigid_body_2d.h"
#include "scene/2d/skeleton_2d.h"
#include "scene/2d/sprite_2d.h"
#include "scene/3d/model.h"
#include "scene/3d/skybox.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/panel.h"
#include "scene/gui/progress_bar.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/stack_container.h"
#include "scene/gui/sub_viewport_container.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/tree.h"
#include "sub_viewport.h"

namespace Flint {

/// Processing order: Input -> Update -> Draw.
class SceneTree {
public:
    SceneTree();

    void input(std::vector<InputEvent> &input_queue) const;

    void update(double dt) const;

    void draw(VkCommandBuffer cmd_buffer) const;

    /// The Root of the scene tree is always a SubViewport.
    std::shared_ptr<Node> get_root() const;

private:
    std::shared_ptr<Node> root;
};

} // namespace Flint

#endif // FLINT_SCENE_TREE_H
