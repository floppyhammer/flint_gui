#include "tree.h"

#include <string>

namespace Flint {
    Tree::Tree() {
        type = NodeType::Panel;

        auto root_ = create_item(nullptr);
        auto child0 = create_item(root_);
        auto child0_0 = create_item(child0);
        auto child1 = create_item(root_);

        root->set_text("root");
    }

    void Tree::update(double delta) {
        Control::update(delta);
    }

    void TreeItem::traverse_children(float folding_width, uint32_t depth, VkCommandBuffer p_command_buffer, float &offset_y, Vec2<float> global_position) {
        float offset_x = (float) depth * folding_width;

        float item_height = label->calculate_minimum_size().y;
        label->set_position(Vec2<float>(offset_x, offset_y) + global_position);
        label->update(0);
        label->draw(p_command_buffer);

        offset_y += item_height;

        for (auto &child: children) {
            child->traverse_children(folding_width, depth + 1, p_command_buffer, offset_y, global_position);
        }
    }

    void Tree::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton().canvas;

        if (theme_panel.has_value()) {
            theme_panel.value().add_to_canvas(get_global_position(), size, canvas);
        }

        float offset_y = 0;
        root->traverse_children(folding_width, 0, p_command_buffer, offset_y, get_global_position());

        outline.add_to_canvas(get_global_position(), size, canvas);
    }

    std::shared_ptr<TreeItem> Tree::create_item(const std::shared_ptr<TreeItem> &parent,
                                                const std::string &text) {
        if (parent == nullptr) {
            root = std::make_shared<TreeItem>();
            root->set_text(text);
            root->label->set_parent(this);
            return root;
        }

        auto item = std::make_shared<TreeItem>();
        item->set_text(text);
        parent->add_child(item);
        item->parent = parent.get();
        item->label->set_parent(this);

        return item;
    }

    TreeItem::TreeItem() {
        label = std::make_shared<Label>();
    }

    uint32_t TreeItem::add_child(const std::shared_ptr<TreeItem>& item) {
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

    void TreeItem::set_text(std::string text) {
        label->set_text(text);
    }
}
