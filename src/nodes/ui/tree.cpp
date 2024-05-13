#include "tree.h"

#include <string>

#include "../../common/utils.h"

namespace Flint {

Tree::Tree() {
    type = NodeType::Tree;

    auto panel = StyleBox();
    panel.bg_color = ColorU(0, 0, 0, 0);
    panel.corner_radius = 4;
    theme_bg = std::make_optional(panel);
    panel.border_width = 2;
    theme_bg_focused = std::make_optional(panel);
}

void Tree::update(double dt) {
    NodeUi::update(dt);
}

void Tree::draw() {
    auto vector_server = VectorServer::get_singleton();

    if (theme_bg.has_value()) {
        vector_server->draw_style_box(theme_bg.value(), get_global_position(), size);
    }

    float offset_y = 0;
    root->propagate_draw(folding_width, 0, offset_y, get_global_position());

    vector_server->draw_style_box(debug_size_box, get_global_position(), size);
}

void Tree::input(InputEvent &event) {
    root->propagate_input(event, get_global_position());

    NodeUi::input(event);
}

std::shared_ptr<TreeItem> Tree::create_item(const std::shared_ptr<TreeItem> &parent, const std::string &text) {
    if (parent == nullptr) {
        root = std::make_shared<TreeItem>();
        root->set_text(text);
        root->tree = this;
        return root;
    }

    auto item = std::make_shared<TreeItem>();
    item->set_text(text);
    parent->add_child(item);
    item->parent = parent.get();
    item->tree = this;

    return item;
}

void Tree::set_item_height(float new_item_height) {
    item_height = new_item_height;
}

float Tree::get_item_height() {
    return item_height;
}

TreeItem::TreeItem() {
    label = std::make_shared<Label>();

    icon = std::make_shared<TextureRect>();
    icon->set_custom_minimum_size({24, 24});
    icon->set_stretch_mode(TextureRect::StretchMode::KeepAspectCentered);

    collapsed_tex = std::make_shared<VectorImage>("../assets/icons/ArrowRight.svg");
    expanded_tex = std::make_shared<VectorImage>("../assets/icons/ArrowDown.svg");

    collapse_button = std::make_shared<Button>();
    collapse_button->set_icon_normal(expanded_tex);
    collapse_button->set_text("");
    collapse_button->set_icon_expand(true);
    collapse_button->set_custom_minimum_size({24, 24});
    collapse_button->theme_normal.border_width = 0;
    collapse_button->theme_normal.bg_color = ColorU::transparent_black();

    auto callback = [this] {
        collapsed = !collapsed;
        if (collapsed) {
            collapse_button->set_icon_normal(collapsed_tex);
        } else {
            collapse_button->set_icon_normal(expanded_tex);
        }
    };
    collapse_button->connect_signal("pressed", callback);

    container = std::make_shared<HBoxContainer>();
    container->set_separation(0);
    container->add_child(collapse_button);
    container->add_child(icon);
    container->add_child(label);

    collapse_button->container_sizing.expand_v = true;
    collapse_button->container_sizing.flag_v = ContainerSizingFlag::Fill;

    label->container_sizing.expand_v = true;
    label->container_sizing.flag_v = ContainerSizingFlag::Fill;

    theme_selected.bg_color = ColorU(100, 100, 100, 150);
}

uint32_t TreeItem::add_child(const std::shared_ptr<TreeItem> &item) {
    children.push_back(item);
    return children.size() - 1;
}

std::shared_ptr<TreeItem> TreeItem::get_child(uint32_t idx) {
    if (idx < children.size()) {
        return children[idx];
    }
    Logger::error("Invalid child index!", "TreeItem");
    return nullptr;
}

uint32_t TreeItem::get_child_count() const {
    return children.size();
}

std::vector<std::shared_ptr<TreeItem>> TreeItem::get_children() {
    return children;
}

TreeItem *TreeItem::get_parent() {
    return parent;
}

void TreeItem::propagate_input(InputEvent &event, Vec2F global_position) {
    if (!children.empty()) {
        collapse_button->input(event);
    }

    if (!collapsed) {
        auto it = children.rbegin();
        while (it != children.rend()) {
            (*it)->propagate_input(event, global_position);
            it++;
        }
    }

    input(event, global_position);
}

void TreeItem::propagate_draw(float folding_width, uint32_t depth, float &offset_y, Vec2F global_position) {
    auto vector_server = VectorServer::get_singleton();

    float offset_x = (float)depth * folding_width;

    // Firstly, the item height will be decided by the minimum height of the icon and label.
    float item_height = std::max(label->get_effective_minimum_size().y, icon->get_custom_minimum_size().y);

    // Then the value set by the tree is considered.
    item_height = std::max(tree->get_item_height(), item_height);

    position = {offset_x, offset_y};

    if (tree->selected_item == this) {
        vector_server->draw_style_box(
            theme_selected, Vec2F(0, offset_y) + global_position, {tree->get_size().x, item_height});
    }

    if (children.empty()) {
        // We should make the button invisible by changing the alpha value instead of the visibility.
        // Otherwise, the container layout will change and the intent will be gone.
        collapse_button->modulate = ColorU::transparent_black();

        collapse_button->set_icon_normal(nullptr);
    } else {
        collapse_button->modulate = ColorU::white();
    }

    container->set_position(Vec2F(offset_x, offset_y) + global_position);
    container->set_size({item_height, item_height});

    std::vector<Node *> nodes;
    dfs_preorder_ltr_traversal(container.get(), nodes);
    for (auto &node : nodes) {
        node->update(0);
    }
    // container->propagate_draw();

    offset_y += item_height;

    if (!collapsed) {
        for (auto &child : children) {
            child->propagate_draw(folding_width, depth + 1, offset_y, global_position);
        }
    }
}

void TreeItem::input(InputEvent &event, Vec2F global_position) {
    float item_height = label->get_effective_minimum_size().y;
    auto item_global_rect = (RectF(0, position.y, tree->get_size().x, position.y + item_height) + global_position);

    if (event.type == InputEventType::MouseButton) {
        auto button_event = event.args.mouse_button;

        if (!event.is_consumed() && button_event.pressed) {
            if (item_global_rect.contains_point(button_event.position)) {
                selected = true;
                tree->selected_item = this;
                Logger::verbose("Item selected: " + label->get_text(), "Tree");
            }
        }
    }
}

void TreeItem::set_text(const std::string &text) {
    label->set_text(text);
}

void TreeItem::set_icon(const std::shared_ptr<Image> &texture) {
    icon->set_texture(texture);
}

} // namespace Flint
