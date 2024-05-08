#include "scene_tree.h"

#include <servers/render_server.h>

#include "sub_window.h"

namespace Flint {

SceneTree::SceneTree() {
    // A default root.
    auto node_ui = std::make_shared<NodeUi>();
    node_ui->set_anchor_flag(AnchorFlag::FullRect);

    root = node_ui;
    root->tree_ = this;
}

void propagate_input(Node* node, InputEvent& event) {
    if (!node->get_visibility()) {
        return;
    }

    for (auto& child : node->get_all_children()) {
        if (typeid(*child) == typeid(SubWindow) || !node->get_visibility()) {
            continue;
        }

        // Do not propagate out-of-bounds mouse input events if they are explicitly ignored.
        if (node->is_ui_node()) {
            auto ui_node = dynamic_cast<NodeUi*>(node);

            if (ui_node->ignore_mouse_input_outside_rect()) {
                // Intercept out-of-scope mouse input events.
                auto global_position = ui_node->get_global_position();

                auto active_rect = RectF(global_position, global_position + ui_node->get_size());

                switch (event.type) {
                    case InputEventType::MouseMotion:
                    case InputEventType::MouseButton:
                    case InputEventType::MouseScroll: {
                        if (!active_rect.contains_point(InputServer::get_singleton()->cursor_position)) {
                            continue;
                        }
                    } break;
                    default:
                        break;
                }
            }
        }

        propagate_input(child.get(), event);
    }

    node->input(event);
}

void input_system(Node* root, std::vector<InputEvent>& input_queue) {
    // Collect all sub-windows.
    std::vector<SubWindow*> sub_windows;
    {
        std::vector<Node*> nodes;
        dfs_preorder_ltr_traversal(root, nodes);

        for (auto& node : nodes) {
            if (typeid(*node) == typeid(SubWindow)) {
                auto sub_window = dynamic_cast<SubWindow*>(node);
                sub_windows.push_back(sub_window);
            }
        }
    }

    for (auto& w : sub_windows) {
        if (!w->get_visibility()) {
            continue;
        }

        for (auto& event : input_queue) {
            if (event.window != w->get_raw_window()->get_glfw_handle()) {
                continue;
            }

            propagate_input(w, event);
        }
    }

    for (auto& event : input_queue) {
        // if (event.window != root->get_window()->get_glfw_handle()) {
        //     continue;
        // }

        propagate_input(root, event);
    }
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

void transform_system(Node* root) {
    if (root == nullptr) {
        return;
    }

    // Collect all orphan UI nodes.
    std::vector<Node*> nodes;
    std::vector<NodeUi*> orphan_ui_nodes;
    dfs_preorder_ltr_traversal(root, nodes);
    for (auto& node : nodes) {
        if (node->is_ui_node()) {
            // Has no parent or no UI parent.
            if (node->get_parent() == nullptr || !node->get_parent()->is_ui_node()) {
                auto ui_node = dynamic_cast<NodeUi*>(node);
                orphan_ui_nodes.push_back(ui_node);
            }
        }
    }

    for (auto& ui_node : orphan_ui_nodes) {
        propagate_transform(ui_node, Vec2F{});
    }
}

void propagate_draw(Node* node) {
    node->draw();

    node->pre_draw_children();

    for (auto& child : node->get_all_children()) {
        if (typeid(*child) == typeid(SubWindow) || !node->get_visibility()) {
            continue;
        }

        propagate_draw(child.get());
    }

    node->post_draw_children();
}

void draw_system(Node* root) {
    // Collect all sub-windows.

    std::vector<SubWindow*> sub_windows;
    {
        std::vector<Node*> nodes;
        dfs_preorder_ltr_traversal(root, nodes);
        for (auto& node : nodes) {
            if (typeid(*node) == typeid(SubWindow)) {
                auto sub_window = dynamic_cast<SubWindow*>(node);
                sub_windows.push_back(sub_window);
            }
        }
    }

    // Draw sub-windows.
    for (auto& w : sub_windows) {
        if (!w->get_visibility()) {
            continue;
        }

        w->pre_draw_children();

        // DRAW
        propagate_draw(w);

        w->post_draw_children();
    }

    propagate_draw(root);
}

void SceneTree::process(double dt) {
    if (root == nullptr) {
        return;
    }

    if (primary_window.lock() && old_primary_window_size != primary_window.lock()->get_size()) {
        old_primary_window_size = primary_window.lock()->get_size();
        when_primary_window_size_changed(old_primary_window_size);
    }

    // Get ready from-back-to-front.
    {
        std::vector<Node*> nodes;
        dfs_preorder_ltr_traversal(root.get(), nodes);
        for (auto& node : nodes) {
            node->ready();
        }
    }

    input_system(root.get(), InputServer::get_singleton()->input_queue);

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

    // Draw from-back-to-front.
    draw_system(root.get());
}

void SceneTree::when_primary_window_size_changed(Vec2I new_size) const {
    root->when_parent_size_changed(new_size.to_f32());
}

std::shared_ptr<Node> SceneTree::replace_root(const std::shared_ptr<Node>& new_root) {
    auto old_root = root;
    old_root->tree_ = nullptr;

    root = new_root;
    root->tree_ = this;

    return old_root;
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
