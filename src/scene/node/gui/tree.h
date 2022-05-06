#ifndef FLINT_NODE_TREE_H
#define FLINT_NODE_TREE_H

#include "control.h"
#include "label.h"
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

        uint32_t add_child(const std::shared_ptr<TreeItem>& item);

        std::shared_ptr<TreeItem> get_child(uint32_t idx);

        uint32_t get_child_count() const;

        std::vector<std::shared_ptr<TreeItem>> get_children();

        TreeItem *get_parent();

        void set_text(std::string text);

        void traverse_children(float folding_width, uint32_t depth, VkCommandBuffer p_command_buffer, float &offset_y, Vec2<float> global_position);

    private:
        std::shared_ptr<Label> label;
        TreeItem *parent;
        std::vector<std::shared_ptr<TreeItem>> children;
    };

    class Tree : public Control {
    public:
        Tree();

        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        float folding_width = 24;

        std::shared_ptr<TreeItem> create_item(const std::shared_ptr<TreeItem> &parent,
                                              const std::string &text = "item");

    private:
        std::shared_ptr<TreeItem> root;
        std::optional<StyleBox> theme_panel;
    };
}

#endif //FLINT_NODE_TREE_H
