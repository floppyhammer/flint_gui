#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "2d/camera2d.h"
#include "2d/skeleton2d.h"
#include "2d/sprite2d.h"
#include "3d/camera3d.h"
#include "3d/model.h"
#include "3d/skybox.h"
#include "file_dialog.h"
#include "node.h"
#include "ui/button.h"
#include "ui/label.h"
#include "ui/margin_container.h"
#include "ui/panel.h"
#include "ui/progress_bar.h"
#include "ui/scroll_container.h"
#include "ui/spin_box.h"
#include "ui/stack_container.h"
#include "ui/tab_container.h"
#include "ui/text_edit.h"
#include "ui/texture_rect.h"
#include "ui/tree.h"
#include "world.h"

namespace Flint {

/// Processing order: Input -> Update -> Draw.
class SceneTree {
public:
    SceneTree();

    void input(std::vector<InputEvent> &input_queue) const;

    void update(double dt) const;

    void draw(VkCommandBuffer cmd_buffer) const;

    void replace_scene(std::shared_ptr<Node> new_scene);

private:
    std::shared_ptr<Node> root;
};

} // namespace Flint

#endif // FLINT_SCENE_TREE_H
