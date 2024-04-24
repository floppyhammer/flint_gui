#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "file_dialog.h"
#include "node.h"
#include "ui/button.h"
#include "ui/check_button.h"
#include "ui/container/box_container.h"
#include "ui/container/margin_container.h"
#include "ui/container/scroll_container.h"
#include "ui/container/tab_container.h"
#include "ui/label.h"
#include "ui/panel.h"
#include "ui/popup_menu.h"
#include "ui/progress_bar.h"
#include "ui/spin_box.h"
#include "ui/text_edit.h"
#include "ui/texture_rect.h"
#include "ui/tree.h"

namespace Pathfinder {
class Window;
}

namespace Flint {

/// Processing order: Input -> Update -> Draw.
class SceneTree {
    friend class App;

public:
    explicit SceneTree();

    void process(double dt);

    void replace_scene(const std::shared_ptr<Node>& new_scene);

    std::shared_ptr<Node> get_root() const;

    void when_primary_window_size_changed(Vec2I new_size) const;

    void quit();

    bool has_quited() const;

private:
    std::shared_ptr<Pathfinder::Window> primary_window;
    Vec2I old_primary_window_size;

    std::shared_ptr<Node> root;

    bool quited = false;
};

} // namespace Flint

#endif // FLINT_SCENE_TREE_H
