#ifndef FLINT_SCENE_TREE_H
#define FLINT_SCENE_TREE_H

#include "file_dialog.h"
#include "node.h"
#include "timer.h"
#include "ui/button.h"
#include "ui/check_button.h"
#include "ui/container/box_container.h"
#include "ui/container/collapse_container.h"
#include "ui/container/margin_container.h"
#include "ui/container/scroll_container.h"
#include "ui/container/tab_container.h"
#include "ui/label.h"
#include "ui/menu_button.h"
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

void transform_system(Node* root);

void draw_system(Node* root);

/// Run calc_minimum_size() depth-first.
void calc_minimum_size(Node* root);

/// Processing order: Input -> Update -> Draw.
class SceneTree {
    friend class App;

public:
    explicit SceneTree();

    void process(double dt);

    std::shared_ptr<Node> replace_root(const std::shared_ptr<Node>& new_root);

    std::shared_ptr<Node> get_root() const;

    void notify_primary_window_size_changed(Vec2I new_size) const;

    void quit();

    bool has_quited() const;

    std::weak_ptr<Pathfinder::Window> get_primary_window() const;

private:
    std::shared_ptr<Node> root;

    bool quited = false;
};

} // namespace Flint

#endif // FLINT_SCENE_TREE_H
