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

        auto root_ = create_item(nullptr);
        auto child0 = create_item(root_);
        auto child0_0 = create_item(child0);
        auto child1 = create_item(root_);

        root->set_text("root");
    }

    void Tree::update(double delta) {
        Control::update(delta);
    }

    void TreeItem::propagate_draw(float folding_width, uint32_t depth, VkCommandBuffer p_command_buffer, float &offset_y,
                                  Vec2<float> global_position) {
        auto canvas = VectorServer::get_singleton().canvas;

        float offset_x = (float) depth * folding_width;

        float item_height = label->calculate_minimum_size().y;

        position = {offset_x, offset_y};

        if (tree->selected_item == this) {
            theme_selected.add_to_canvas(Vec2<float>(0, offset_y) + global_position, {tree->get_size().x, item_height}, canvas);
        }

        // The attached label has no parent.
        label->set_position(Vec2<float>(offset_x, offset_y) + global_position);
        label->update(0);
        label->draw(p_command_buffer);

        offset_y += item_height;

        for (auto &child: children) {
            child->propagate_draw(folding_width, depth + 1, p_command_buffer, offset_y, global_position);
        }
    }

    void Tree::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton().canvas;

        if (theme_bg.has_value()) {
            theme_bg.value().add_to_canvas(get_global_position(), size, canvas);
        }

        float offset_y = 0;
        root->propagate_draw(folding_width, 0, p_command_buffer, offset_y, get_global_position());

        outline.add_to_canvas(get_global_position(), size, canvas);
    }

    std::shared_ptr<TreeItem> Tree::create_item(const std::shared_ptr<TreeItem> &parent,
                                                const std::string &text) {
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

    void Tree::input(std::vector<InputEvent> &input_queue) {
        root->propagate_input(input_queue, get_global_position());

        Control::input(input_queue);
    }

    TreeItem::TreeItem() {
        label = std::make_shared<Label>();

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

    void TreeItem::propagate_input(std::vector<InputEvent> &input_queue,
                                   Vec2<float> global_position) {
        auto canvas = VectorServer::get_singleton().canvas;

        auto it = children.rbegin();
        while (it != children.rend()) {
            (*it)->propagate_input(input_queue, global_position);
            it++;
        }

        input(input_queue, global_position);
    }

    void TreeItem::input(std::vector<InputEvent> &input_queue,
                         Vec2<float> global_position) {
        float item_height = label->calculate_minimum_size().y;
        auto item_global_rect = (Rect<float>(0, position.y, tree->get_size().x, position.y + item_height) + global_position);

        for (auto &event: input_queue) {
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
    }

    void TreeItem::set_text(const std::string &text) {
        label->set_text(text);
    }
}
