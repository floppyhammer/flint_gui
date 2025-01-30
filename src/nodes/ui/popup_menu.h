#ifndef FLINT_NODE_POPUPMENU_H
#define FLINT_NODE_POPUPMENU_H

#include <cstdint>
#include <memory>
#include <optional>

#include "../../common/geometry.h"
#include "../../resources/font.h"
#include "../../resources/style_box.h"
#include "../../resources/vector_image.h"
#include "container/box_container.h"
#include "label.h"
#include "node_ui.h"
#include "texture_rect.h"

namespace Flint {

class PopupMenu;

class ScrollContainer;

class Button;

class MarginContainer;

// class MenuItem {
//     friend class PopupMenu;
//
// public:
//     MenuItem();
//
//     void update(Vec2F global_position, Vec2F p_size);
//
//     void input(InputEvent &event, Vec2F global_position);
//
//     void draw(Vec2F global_position);
//
//     void set_text(const std::string &text);
//
//     void set_icon(const std::shared_ptr<Image> &image);
//
// public:
//     bool hovered = false;
//
//     // Expanded sub menu.
//     bool expanded = false;
//
//     // Local position in the menu.
//     Vec2F position;
//
//     Vec2F size;
//
//     std::shared_ptr<TextureRect> icon;
//
//     std::shared_ptr<Label> label;
//
//     std::shared_ptr<TextureRect> expand_icon;
//
//     std::shared_ptr<HBoxContainer> container;
//
//     std::shared_ptr<PopupMenu> sub_menu;
//
//     StyleBox theme_hovered;
// };

// TODO: we should make it flexible to change a popup menu's parent from a NodeUi to a SubWindow,
// so we can have intuitive windowed popup support.
class PopupMenu : public NodeUi {
public:
    PopupMenu();

    void input(InputEvent &event) override;

    void update(double dt) override;

    void draw() override;

    void clear_items();

    void set_visibility(bool visible) override;

    void create_item(const std::string &text = "item");

    void set_item_height(float new_item_height);

    float get_item_height() const;

    int get_item_count() const;

    std::string get_item_text(uint32_t item_index) const;

    void connect_signal(const std::string &signal, const AnyCallable<void> &callback) override;

    void calc_minimum_size() override;

private:
    void when_item_selected(uint32_t item_index);
    void when_popup_hide();

    std::shared_ptr<ScrollContainer> scroll_container_;

    std::shared_ptr<VBoxContainer> vbox_container_;
    std::shared_ptr<MarginContainer> margin_container_;

    std::vector<std::shared_ptr<Button>> items_;

    float item_height_ = 48;

    std::optional<StyleBox> theme_bg_;

    std::vector<AnyCallable<void>> item_selected_callbacks;

    std::vector<AnyCallable<void>> popup_hide_callbacks;
};

} // namespace Flint

#endif // FLINT_NODE_POPUPMENU_H
