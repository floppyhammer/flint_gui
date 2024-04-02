#ifndef FLINT_NODE_POPUPMENU_H
#define FLINT_NODE_POPUPMENU_H

#include <cstdint>
#include <memory>
#include <optional>

#include "../../common/geometry.h"
#include "../../resources/font.h"
#include "../../resources/style_box.h"
#include "../../resources/vector_image.h"
#include "container/stack_container.h"
#include "label.h"
#include "node_ui.h"
#include "texture_rect.h"

namespace Flint {

class PopupMenu;

class MenuItem {
    friend class PopupMenu;

public:
    MenuItem();

    void update(Vec2F global_position, Vec2F size);

    void input(InputEvent &event, Vec2F global_position);

    void draw(Vec2F global_position);

    void set_text(const std::string &text);

    void set_icon(const std::shared_ptr<Image> &image);

private:
    bool hovered = false;

    // Expanded sub menu.
    bool expanded = false;

    // Local position in the menu.
    Vec2F position;
    Vec2F size;

    std::shared_ptr<TextureRect> icon;

    std::shared_ptr<Label> label;

    std::shared_ptr<TextureRect> expand_icon;

    std::shared_ptr<HStackContainer> container;

    std::shared_ptr<PopupMenu> sub_menu;

    StyleBox theme_hovered;
};

// TODO: we should make it flexible to change a popup menu's parent from a NodeUi to a SubWindow,
// so we can have intuitive windowed popup support.
class PopupMenu : public NodeUi {
public:
    PopupMenu();

    void input(InputEvent &event) override;

    void update(double delta) override;

    void draw() override;

    std::shared_ptr<MenuItem> create_item(const std::string &text = "item");

    void set_item_height(float new_item_height);

    float get_item_height() const;

private:
    std::vector<std::shared_ptr<MenuItem>> items_;

    float item_height_ = 48;

    std::optional<StyleBox> theme_bg_;
};

} // namespace Flint

#endif // FLINT_NODE_POPUPMENU_H
