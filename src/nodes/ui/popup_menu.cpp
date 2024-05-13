#include "popup_menu.h"

#include <string>

#include "../../common/utils.h"

namespace Flint {

MenuItem::MenuItem() {
    label = std::make_shared<Label>();

    label->container_sizing.expand_v = true;
    label->container_sizing.flag_v = ContainerSizingFlag::ShrinkCenter;
    label->set_vertical_alignment(Alignment::Center);

    icon = std::make_shared<TextureRect>();
    icon->set_custom_minimum_size({24, 24});
    icon->set_stretch_mode(TextureRect::StretchMode::KeepAspectCentered);

    // auto img = std::make_shared<VectorImage>("../assets/icons/ArrowRight.svg");
    // icon->set_texture(img);

    container = std::make_shared<HBoxContainer>();
    container->set_separation(0);
    container->add_child(icon);
    container->add_child(label);

    theme_hovered.bg_color = ColorU(100, 100, 100, 150);
}

void MenuItem::draw(Vec2F global_position) {
    auto vector_server = VectorServer::get_singleton();

    if (hovered) {
        vector_server->draw_style_box(theme_hovered, global_position + position, container->get_size());
    }
}

void MenuItem::update(Vec2F global_position, Vec2F size) {
    size = size.max(container->get_effective_minimum_size());

    container->set_position(global_position + position);
    container->set_size(size);

    std::vector<Node *> descendants;
    dfs_preorder_ltr_traversal(container.get(), descendants);
    for (auto &node : descendants) {
        node->update(0);
    }
}

void MenuItem::input(InputEvent &event, Vec2F global_position) {
    float item_height = label->get_effective_minimum_size().y;
    auto item_global_rect = (RectF(0, position.y, container->get_size().x, position.y + item_height) + global_position);

    // if (event.type == InputEventType::MouseMotion) {
    //     auto button_event = event.args.mouse_motion;
    //
    //     if (!event.is_consumed()) {
    //         if (item_global_rect.contains_point(button_event.position)) {
    //             hovered = true;
    //             event.consume();
    //         } else {
    //             hovered = false;
    //         }
    //     }
    // }
}

void MenuItem::set_text(const std::string &text) {
    label->set_text(text);
}

void MenuItem::set_icon(const std::shared_ptr<Image> &texture) {
    icon->set_texture(texture);
}

PopupMenu::PopupMenu() {
    type = NodeType::PopupMenu;

    auto panel = StyleBox();
    panel.bg_color = ColorU(39, 39, 39, 255);
    panel.corner_radius = 8;
    panel.border_width = 2;

    theme_bg_ = std::make_optional(panel);

    debug_size_box.border_color = ColorU(100, 40, 122, 255);
}

void PopupMenu::update(double delta) {
    if (!visible_) {
        return;
    }

    auto global_position = get_global_position();

    float offset_y = 0;
    for (auto &item : items_) {
        item->position = {0, offset_y};
        offset_y += item_height_;
        item->update(global_position, {size.x, item_height_});
    }

    size = size.max(custom_minimum_size.max(Vec2F{0, offset_y}));

    NodeUi::update(delta);
}

void PopupMenu::draw() {
    if (!visible_) {
        return;
    }

    NodeUi::draw();

    auto vector_server = VectorServer::get_singleton();

    if (theme_bg_.has_value()) {
        vector_server->draw_style_box(theme_bg_.value(), get_global_position(), size);
    }

    for (auto &item : items_) {
        item->draw(get_global_position());
    }
}

void PopupMenu::input(InputEvent &event) {
    if (!visible_) {
        return;
    }

    auto global_position = get_global_position();

    bool consume_flag = false;

    if (event.type == InputEventType::MouseMotion) {
        auto args = event.args.mouse_motion;
        auto local_mouse_position = args.position - global_position;

        for (auto &item : items_) {
            item->hovered = false;
        }

        if (RectF(global_position, global_position + size).contains_point(args.position)) {
            int item_index = int(local_mouse_position.y / item_height_);
            item_index = std::clamp(item_index, 0, (int)items_.size() - 1);
            items_[item_index]->hovered = true;
        } else {
        }
    }

    for (auto &item : items_) {
        item->input(event, global_position);
    }

    NodeUi::input(event);
}

std::shared_ptr<MenuItem> PopupMenu::create_item(const std::string &text) {
    auto new_item = std::make_shared<MenuItem>();
    new_item->set_text(text);
    items_.push_back(new_item);
    return new_item;
}

void PopupMenu::set_item_height(float new_item_height) {
    item_height_ = new_item_height;
}

float PopupMenu::get_item_height() const {
    return item_height_;
}

} // namespace Flint
