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

    void Tree::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton().canvas;

        if (theme_bg.has_value()) {
            theme_bg.value().add_to_canvas(get_global_position(), size, canvas);
        }

        float offset_y = 0;
        root->propagate_draw(folding_width, 0, p_command_buffer, offset_y, get_global_position());

        outline.add_to_canvas(get_global_position(), size, canvas);
    }

    void Tree::input(std::vector<InputEvent> &input_queue) {
        root->propagate_input(input_queue, get_global_position());

        Control::input(input_queue);
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

    TreeItem::TreeItem() {
        label = std::make_shared<Label>();

        {
            collapse_icon = VectorTexture::from_empty(24, 24);
            SvgShape svg_shape;
            svg_shape.shape.move_to(6, -6);
            svg_shape.shape.line_to(0, 6);
            svg_shape.shape.line_to(-6, -6);
            svg_shape.shape.close();
            svg_shape.shape.translate({collapse_icon->get_width() * 0.5f, collapse_icon->get_height() * 0.5f});
            svg_shape.stroke_color = ColorU(163, 163, 163, 255);
            svg_shape.stroke_width = 2;
            collapse_icon->add_svg_shape(svg_shape);
        }

        {
            expand_icon = VectorTexture::from_empty(24, 24);
            SvgShape svg_shape;
            svg_shape.shape.move_to(-6, -6);
            svg_shape.shape.line_to(6, 0);
            svg_shape.shape.line_to(-6, 6);
            svg_shape.shape.close();
            svg_shape.shape.translate({expand_icon->get_width() * 0.5f, expand_icon->get_height() * 0.5f});
            svg_shape.stroke_color = ColorU(163, 163, 163, 255);
            svg_shape.stroke_width = 2;
            expand_icon->add_svg_shape(svg_shape);
        }

        collapse_button = std::make_shared<Button>();
        collapse_button->set_icon(collapse_icon);
        collapse_button->set_text("");
        collapse_button->set_size({24, 24});

        auto callback = [this] {
            collapsed = !collapsed;
            if (collapsed) {
                collapse_button->set_icon(expand_icon);
            } else {
                collapse_button->set_icon(collapse_icon);
            }
        };
        collapse_button->connect_signal("on_pressed", callback);

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

        if (!children.empty())
            collapse_button->input(input_queue);

        if (!collapsed) {
            auto it = children.rbegin();
            while (it != children.rend()) {
                (*it)->propagate_input(input_queue, global_position);
                it++;
            }
        }

        input(input_queue, global_position);
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

        collapse_button->set_position(Vec2<float>(offset_x, offset_y) + global_position);
        collapse_button->set_size({item_height, item_height});
        collapse_button->update(0);
        if (!children.empty()) {
            collapse_button->draw(p_command_buffer);
        }

        // The attached label has no parent.
        label->set_position(Vec2<float>(offset_x + collapse_button->get_size().x, offset_y) + global_position);
        label->set_size(label->calculate_minimum_size());
        label->update(0);
        label->draw(p_command_buffer);

        offset_y += item_height;

        if (!collapsed) {
            for (auto &child: children) {
                child->propagate_draw(folding_width, depth + 1, p_command_buffer, offset_y, global_position);
            }
        }
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
