#include <iostream>
#include <random>

#include "app.h"

using namespace Flint;

using Pathfinder::Vec2;
using Pathfinder::Vec3;

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

class MyNode : public Node {
    void custom_ready() override {
        auto panel = std::make_shared<Panel>();
        panel->set_position({200, 200});
        panel->set_size({400, 300});
        add_child(panel);

        auto tree = std::make_shared<Tree>();
        tree->set_anchor_flag(AnchorFlag::FullRect);
        panel->add_child(tree);

        // Set up tree items.
        {
            auto resource_mgr = ResourceManager::get_singleton();

            auto tree_root = tree->create_item(nullptr, "Node");
            tree_root->set_icon(resource_mgr->load<VectorImage>("../assets/icons/Node_Node.svg"));
            auto child_ui = tree->create_item(tree_root, "NodeUi");
            child_ui->set_icon(resource_mgr->load<VectorImage>("../assets/icons/Node_Control.svg"));
            auto child_label = tree->create_item(child_ui, "Label");
            child_label->set_icon(resource_mgr->load<VectorImage>("../assets/icons/Node_Label.svg"));
            auto child_text_edit = tree->create_item(child_ui, "TextEdit");
            child_text_edit->set_icon(resource_mgr->load<VectorImage>("../assets/icons/Node_LineEdit.svg"));
            auto child_node_2d = tree->create_item(tree_root, "Node2d");
            child_node_2d->set_icon(resource_mgr->load<VectorImage>("../assets/icons/Node_Node2D.svg"));
            auto child_node_3d = tree->create_item(tree_root, "Node3d");
            child_node_3d->set_icon(resource_mgr->load<VectorImage>("../assets/icons/Node_Node3D.svg"));
        }
    }
};

int main() {
    App app({WINDOW_WIDTH, WINDOW_HEIGHT});

    app.get_tree()->replace_root(std::make_shared<MyNode>());

    app.main_loop();

    return EXIT_SUCCESS;
}
