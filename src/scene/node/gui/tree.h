#ifndef FLINT_NODE_TREE_H
#define FLINT_NODE_TREE_H

#include "control.h"
#include "button.h"
#include "../../../resources/style_box.h"
#include "../../../resources/font.h"
#include "../../../resources/vector_texture.h"
#include "../../../common/geometry.h"

#include <cstdint>
#include <memory>

namespace Flint {
    class TreeItem {
        friend class Tree;

    public:
        TreeItem();

        uint32_t add_child(const std::shared_ptr<TreeItem> &item);

        std::shared_ptr<TreeItem> get_child(uint32_t idx);

        uint32_t get_child_count() const;

        std::vector<std::shared_ptr<TreeItem>> get_children();

        TreeItem *get_parent();

        void propagate_input(std::vector<InputEvent> &input_queue,
                             Vec2<float> global_position);

        void input(std::vector<InputEvent> &input_queue,
                   Vec2<float> global_position);

        void propagate_draw(float folding_width, uint32_t depth, VkCommandBuffer p_command_buffer, float &offset_y,
                            Vec2<float> global_position);

        void set_text(const std::string &text);

        void set_icon(const std::shared_ptr<Texture> &texture);

    private:
        bool collapsed = false;
        bool selected = false;

        // Local position in the tree.
        Vec2<float> position;

        std::shared_ptr<Button> collapse_button;
        std::shared_ptr<VectorTexture> collapse_icon, expand_icon;

        std::shared_ptr<TextureRect> icon;

        std::shared_ptr<Label> label;

        std::shared_ptr<BoxContainer> container;

        std::vector<std::shared_ptr<TreeItem>> children;
        TreeItem *parent;

        Tree *tree;

        StyleBox theme_selected;
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

        TreeItem *selected_item{};

    private:
        std::shared_ptr<TreeItem> root;
        std::optional<StyleBox> theme_bg;
        std::optional<StyleBox> theme_bg_focused;
    };
}

#endif //FLINT_NODE_TREE_H
