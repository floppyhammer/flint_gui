#ifndef FLINT_NODE_TREE_H
#define FLINT_NODE_TREE_H

#include "control.h"
#include "button.h"
#include "../../../resources/style_box.h"
#include "../../../resources/font.h"
#include "../../../common/math/rect.h"

#include <cstdint>
#include <memory>

namespace Flint {
    class TreeItem {
        friend class Tree;

    public:
        TreeItem();

        bool collapsed = false;

        uint32_t add_child(const std::shared_ptr<TreeItem> &item);

        std::shared_ptr<TreeItem> get_child(uint32_t idx);

        uint32_t get_child_count() const;

        std::vector<std::shared_ptr<TreeItem>> get_children();

        TreeItem *get_parent();

        void set_text(const std::string &text);

        void traverse_children(float folding_width, uint32_t depth, VkCommandBuffer p_command_buffer, float &offset_y,
                               Vec2<float> global_position);

    private:
        std::shared_ptr<Button> button;
        TreeItem *parent;
        std::vector<std::shared_ptr<TreeItem>> children;
    };

    class Tree : public Control {
    public:
        Tree();

        void input(std::vector<InputEvent> &input_queue) override;

        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        float folding_width = 24;

        std::shared_ptr<TreeItem> create_item(const std::shared_ptr<TreeItem> &parent,
                                              const std::string &text = "item");

    private:
        std::shared_ptr<TreeItem> root;
        std::optional<StyleBox> theme_bg;
        std::optional<StyleBox> theme_bg_focused;
    };
}

#endif //FLINT_NODE_TREE_H
