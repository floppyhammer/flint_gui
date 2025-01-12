#include "popup_menu.h"

#include <string>

#include "../../common/utils.h"
#include "../../resources/default_resource.h"
#include "../scene_tree.h"

namespace Flint {

PopupMenu::PopupMenu() {
    type = NodeType::PopupMenu;

    auto panel = StyleBox();
    panel.bg_color = ColorU(39, 39, 39, 255);
    panel.corner_radius = 8;
    panel.border_width = 2;

    scroll_container_ = std::make_shared<ScrollContainer>();
    scroll_container_->render_layer = 1;
    scroll_container_->set_anchor_flag(AnchorFlag::FullRect);
    add_embedded_child(scroll_container_);

    margin_container_ = std::make_shared<MarginContainer>();
    scroll_container_->add_child(margin_container_);
    auto default_theme = DefaultResource::get_singleton()->get_default_theme();
    theme_bg_ = std::make_optional(default_theme->panel.styles["background"]);

    vbox_container_ = std::make_shared<VBoxContainer>();
    margin_container_->add_child(vbox_container_);

    auto callback = [this] {
        set_visibility(false);
    };
    connect_signal("focus_released", callback);

    theme_bg_ = std::make_optional(panel);

    debug_size_box.border_color = ColorU(100, 40, 122, 255);
}

void PopupMenu::update(double delta) {
    auto global_position = get_global_position();

    // float offset_y = 0;
    // for (auto &item : items_) {
    //     item->position = {0, offset_y};
    //     offset_y += item_height_;
    //     item->update(global_position, {size.x, item_height_});
    // }

    size = size.max(calculated_minimum_size);

    NodeUi::update(delta);
}

void PopupMenu::draw() {
    if (!visible_) {
        return;
    }

    auto vector_server = VectorServer::get_singleton();

    NodeUi::draw();

    if (theme_bg_.has_value()) {
        vector_server->set_render_layer(render_layer);
        vector_server->draw_style_box(theme_bg_.value(), get_global_position(), size);
        vector_server->set_render_layer(0);
    }

    // auto global_position = get_global_position();
    // for (auto &item : items_) {
    //     item->draw(global_position);
    // }
}

void PopupMenu::input(InputEvent &event) {
    if (!visible_) {
        return;
    }

    // auto global_position = get_global_position();
    //
    // // If a popup menu is shown, it captures mouse events anyway.
    // bool consume_flag = true;
    //
    // if (event.type == InputEventType::MouseMotion) {
    //     auto args = event.args.mouse_motion;
    //     auto local_mouse_position = args.position - global_position;
    //
    //     for (auto &item : items_) {
    //         item->hovered = false;
    //     }
    //
    //     if (RectF(global_position, global_position + size).contains_point(args.position)) {
    //         consume_flag = true;
    //
    //         if (items_.empty()) {
    //             return;
    //         }
    //         int item_index = int(local_mouse_position.y / item_height_);
    //         item_index = std::clamp(item_index, 0, (int)items_.size() - 1);
    //         items_[item_index]->hovered = true;
    //     }
    // }
    //
    // if (event.type == InputEventType::MouseButton) {
    //     auto args = event.args.mouse_button;
    //
    //     // Hide menu.
    //     if (RectF(global_position, global_position + size).contains_point(args.position)) {
    //         consume_flag = true;
    //         set_visibility(false);
    //
    //         if (items_.empty()) {
    //             return;
    //         }
    //         int item_index = int(local_mouse_position.y / item_height_);
    //         item_index = std::clamp(item_index, 0, (int)items_.size() - 1);
    //
    //         when_item_selected(item_index);
    //     } else {
    //         consume_flag = true;
    //         set_visibility(false);
    //     }
    // }
    //
    // for (auto &item : items_) {
    //     item->input(event, global_position);
    // }
    //
    // event.consume();

    NodeUi::input(event);
}

void PopupMenu::set_visibility(bool visible) {
    visible_ = visible;
    if (visible_) {
        // TODO: we should not do this manually in here.
        margin_container_->calc_minimum_size_recursively();

        float menu_width = std::max(size.x, margin_container_->get_effective_minimum_size().x);
        float menu_height = std::min(margin_container_->get_effective_minimum_size().y,
                                     get_window()->get_logical_size().y - position.y);
        set_size({menu_width, menu_height});
    } else {
        when_popup_hide();
    }
}

void PopupMenu::clear_items() {
    vbox_container_->remove_all_children();
    items_.clear();
}

void PopupMenu::calc_minimum_size() {
    calculated_minimum_size = {};
}

void PopupMenu::create_item(const std::string &text) {
    auto new_item = std::make_shared<Button>();
    new_item->set_text(text);
    vbox_container_->add_child(new_item);

    int item_index = vbox_container_->get_children().size() - 1;

    auto callback = [item_index, this] {
        set_visibility(false);
        when_item_selected(item_index);
    };
    new_item->connect_signal("pressed", callback);

    items_.push_back(new_item);
}

void PopupMenu::set_item_height(float new_item_height) {
    item_height_ = new_item_height;
    // TODO
}

float PopupMenu::get_item_height() const {
    return item_height_;
}

int PopupMenu::get_item_count() const {
    return items_.size();
}

std::string PopupMenu::get_item_text(uint32_t item_index) const {
    return items_[item_index]->get_text();
}

void PopupMenu::connect_signal(const std::string &signal, const AnyCallable<void> &callback) {
    NodeUi::connect_signal(signal, callback);

    if (signal == "item_selected") {
        item_selected_callbacks.push_back(callback);
    }
    if (signal == "popup_hide") {
        popup_hide_callbacks.push_back(callback);
    }
}

void PopupMenu::when_item_selected(uint32_t item_index) {
    for (auto &callback : item_selected_callbacks) {
        try {
            callback.operator()<uint32_t>(std::move(item_index));
        } catch (std::bad_any_cast &) {
            Logger::error("Mismatched signal argument types!", "Flint");
        }
    }
}

void PopupMenu::when_popup_hide() {
    for (auto &callback : popup_hide_callbacks) {
        try {
            callback();
        } catch (std::bad_any_cast &) {
            Logger::error("Mismatched signal argument types!", "Flint");
        }
    }
}

} // namespace Flint
