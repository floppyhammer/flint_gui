#include "tree.h"

#include <string>

namespace Flint {
Tree::Tree() {
    type = NodeType::Panel;

    auto panel = StyleBox();
    panel.bg_color = ColorU(0, 0, 0, 0);
    panel.corner_radius = 4;
    theme_bg = std::make_optional(panel);
    panel.border_width = 2;
    theme_bg_focused = std::make_optional(panel);

    auto root_ = create_item(nullptr, "Node");
    root->set_icon(ResourceManager::get_singleton()->load<VectorTexture>("../assets/icons/Node_Node.svg"));
    auto child_control = create_item(root_, "Control");
    child_control->set_icon(ResourceManager::get_singleton()->load<VectorTexture>("../assets/icons/Node_Control.svg"));
    auto child_node_2d = create_item(root_, "Node2D");
    child_node_2d->set_icon(ResourceManager::get_singleton()->load<VectorTexture>("../assets/icons/Node_Node2D.svg"));
    auto child_node_3d = create_item(root_, "Node3D");
    child_node_3d->set_icon(ResourceManager::get_singleton()->load<VectorTexture>("../assets/icons/Node_Node3D.svg"));
    auto child_label = create_item(child_control, "Label");
    child_label->set_icon(ResourceManager::get_singleton()->load<VectorTexture>("../assets/icons/Node_Label.svg"));
    auto child_text_edit = create_item(child_control, "TextEdit");
    child_text_edit->set_icon(
        ResourceManager::get_singleton()->load<VectorTexture>("../assets/icons/Node_LineEdit.svg"));
}

void Tree::update(double delta) {
    Control::update(delta);
}

void Tree::draw(VkCommandBuffer p_command_buffer) {
    auto vector_server = VectorServer::get_singleton();

    if (theme_bg.has_value()) {
        vector_server->draw_style_box(theme_bg.value(), get_global_position(), size);
    }

    float offset_y = 0;
    root->propagate_draw(folding_width, 0, p_command_buffer, offset_y, get_global_position());

    vector_server->draw_style_box(debug_size_box, get_global_position(), size);
}

void Tree::input(InputEvent &event) {
    root->propagate_input(event, get_global_position());

    Control::input(event);
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
    label = std::make_shared<Label>("");

    icon = std::make_shared<TextureRect>();
    icon->set_minimum_size({24, 24});
    icon->set_stretch_mode(TextureRect::StretchMode::KeepCentered);

    {
        collapse_icon = VectorTexture::from_empty(24, 24);
        VectorPath vp;
        float x = collapse_icon->get_width() * 0.5f;
        float y = collapse_icon->get_height() * 0.5f;
        vp.path2d.move_to(-6 + x, -3 + y);
        vp.path2d.line_to(0 + x, 3 + y);
        vp.path2d.line_to(6 + x, -3 + y);
        vp.stroke_color = ColorU(163, 163, 163, 255);
        vp.stroke_width = 2;
        collapse_icon->add_path(vp);
    }

    {
        expand_icon = VectorTexture::from_empty(24, 24);
        VectorPath vp;
        float x = expand_icon->get_width() * 0.5f;
        float y = expand_icon->get_height() * 0.5f;
        vp.path2d.move_to(-3 + x, -6 + y);
        vp.path2d.line_to(3 + x, 0 + y);
        vp.path2d.line_to(-3 + x, 6 + y);
        vp.stroke_color = ColorU(163, 163, 163, 255);
        vp.stroke_width = 2;
        expand_icon->add_path(vp);
    }

    collapse_button = std::make_shared<Button>();
    collapse_button->set_icon(collapse_icon);
    collapse_button->set_text("");
    collapse_button->set_minimum_size({24, 24});

    auto callback = [this] {
        collapsed = !collapsed;
        if (collapsed) {
            collapse_button->set_icon(expand_icon);
        } else {
            collapse_button->set_icon(collapse_icon);
        }
    };
    collapse_button->connect_signal("pressed", callback);

    container = std::make_shared<HStackContainer>();
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
    } else {
        Logger::error("Invalid child index!", "TreeItem");
        return nullptr;
    }
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

void TreeItem::propagate_draw(
    float folding_width, uint32_t depth, VkCommandBuffer p_command_buffer, float &offset_y, Vec2F global_position) {
    auto vector_server = VectorServer::get_singleton();

    float offset_x = (float)depth * folding_width;

    // Firstly, the item height will be decided by the minimum height of the icon and label.
    float item_height = std::max(label->calculate_minimum_size().y, icon->get_minimum_size().y);

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

        collapse_button->set_icon(nullptr);
    } else {
        collapse_button->modulate = ColorU::white();
    }

    container->set_position(Vec2F(offset_x, offset_y) + global_position);
    container->set_size({item_height, item_height});
    container->propagate_update(0);
    container->propagate_draw(p_command_buffer);

    offset_y += item_height;

    if (!collapsed) {
        for (auto &child : children) {
            child->propagate_draw(folding_width, depth + 1, p_command_buffer, offset_y, global_position);
        }
    }
}

void TreeItem::input(InputEvent &event, Vec2F global_position) {
    float item_height = label->calculate_minimum_size().y;
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

void TreeItem::set_icon(const std::shared_ptr<Texture> &texture) {
    icon->set_texture(texture);
}

} // namespace Flint
