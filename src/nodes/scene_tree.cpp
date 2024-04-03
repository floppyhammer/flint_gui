#include "scene_tree.h"

namespace Flint {

SceneTree::SceneTree() {
    // A default root.
    auto node_ui = std::make_shared<NodeUi>();
    node_ui->set_anchor_flag(AnchorFlag::FullRect);

    root = node_ui;
    root->tree_ = this;
}

void SceneTree::replace_scene(const std::shared_ptr<Node>& new_scene) {
    root = new_scene;
    root->tree_ = this;
}

void propagate_transform(NodeUi* node, Vec2F parent_global_transform) {
    if (node == nullptr) {
        return;
    }

    node->calc_global_position(parent_global_transform);

    for (auto& child : node->get_all_children()) {
        if (child->is_ui_node()) {
            auto ui_child = dynamic_cast<NodeUi*>(child.get());
            propagate_transform(ui_child, node->get_global_position());
        }
    }
}

void transform_system(Node* node) {
    if (node == nullptr) {
        return;
    }

    // Collect all orphan UI nodes.
    std::vector<Node*> nodes;
    std::vector<NodeUi*> orphan_ui_nodes;
    dfs_preorder_ltr_traversal(node, nodes);
    for (auto& node : nodes) {
        if (node->is_ui_node() && node->get_parent() == nullptr) {
            auto ui_node = dynamic_cast<NodeUi*>(node);
            orphan_ui_nodes.push_back(ui_node);
        }
    }

    for (auto& ui_node : orphan_ui_nodes) {
        propagate_transform(ui_node, Vec2F{});
    }
}

void SceneTree::process(double dt) {
    if (root == nullptr) {
        return;
    }

    if (primary_window && old_primary_window_size != primary_window->get_size()) {
        old_primary_window_size = primary_window->get_size();
        when_primary_window_size_changed(old_primary_window_size);
    }

    for (auto& event : InputServer::get_singleton()->input_queue) {
        root->propagate_input(event);
    }

    // Run calc_minimum_size() depth-first.
    {
        std::vector<Node*> descendants;
        dfs_postorder_ltr_traversal(root.get(), descendants);
        for (auto& node : descendants) {
            if (node->is_ui_node()) {
                auto ui_node = dynamic_cast<NodeUi*>(node);
                ui_node->calc_minimum_size();
                // std::cout << "Node: " << get_node_type_name(node->type)
                //           << ", size: " << ui_node->get_effective_minimum_size() << std::endl;
            }
        }
    }

    // Update global transform.
    transform_system(root.get());

    // Update from-back-to-front.
    {
        std::vector<Node*> nodes;
        dfs_preorder_ltr_traversal(root.get(), nodes);
        for (auto& node : nodes) {
            node->update(dt);
        }
    }

    // Collect draw commands.
    root->propagate_draw();
}

void SceneTree::when_primary_window_size_changed(Vec2I new_size) const {
    root->when_parent_size_changed(new_size.to_f32());
}

std::shared_ptr<Node> SceneTree::get_root() const {
    return root;
}

void SceneTree::quit() {
    quited = true;
}

bool SceneTree::has_quited() const {
    return quited;
}

} // namespace Flint
