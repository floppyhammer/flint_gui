#include "node.h"

#include <string>

#include "../servers/render_server.h"
#include "sub_window.h"

namespace Flint {

const char *NodeNames[] = {
    "Node",
    "Window",

    "NodeUi",

    "Container",
    "CenterContainer",
    "MarginContainer",
    "HBoxContainer",
    "VBoxContainer",
    "ScrollContainer",
    "TabContainer",

    "Button",
    "MenuButton",
    "OptionButton",
    "CheckButton",
    "RadioButton",

    "Label",
    "TextEdit",
    "SpinBox",
    "Panel",
    "TextureRect",
    "Tree",
    "ProgressBar",
    "PopupMenu",

    "NotInstantiable",

    "Max",
};

std::string get_node_type_name(NodeType type) {
    return NodeNames[(uint32_t)type];
}

void dfs_preorder_ltr_traversal(Node *node, std::vector<Node *> &ordered_nodes) {
    if (node == nullptr) {
        return;
    }

    // Debug print.
    // std::cout << "Node: " << get_node_type_name(node->type) << std::endl;

    ordered_nodes.push_back(node);

    for (auto &child : node->get_all_children()) {
        dfs_preorder_ltr_traversal(child.get(), ordered_nodes);
    }
}

void dfs_postorder_ltr_traversal(Node *node, std::vector<Node *> &ordered_nodes) {
    if (node == nullptr) {
        return;
    }

    for (auto &child : node->get_all_children()) {
        dfs_postorder_ltr_traversal(child.get(), ordered_nodes);
    }

    // Debug print.
    // std::cout << "Node: " << get_node_type_name(node->type) << std::endl;

    ordered_nodes.push_back(node);
}

void dfs_postorder_rtl_traversal(Node *node, std::vector<Node *> &ordered_nodes) {
    if (node == nullptr) {
        return;
    }

    for (auto riter = node->get_all_children().rbegin(); riter != node->get_all_children().rend(); ++riter) {
        dfs_postorder_rtl_traversal(riter->get(), ordered_nodes);
    }

    // Debug print.
    // std::cout << "Node: " << get_node_type_name(node->type) << std::endl;

    ordered_nodes.push_back(node);
}

void Node::input(InputEvent &event) {
    custom_input(event);
}

void Node::update(double dt) {
    custom_update(dt);
}

void Node::notify(Signal signal) {
}

void Node::draw() {
    custom_draw();
}

Node *Node::get_parent() const {
    return parent;
}

std::vector<std::shared_ptr<Node>> Node::get_children() {
    return children;
}

std::vector<std::shared_ptr<Node>> Node::get_embedded_children() {
    return embedded_children;
}

std::vector<std::shared_ptr<Node>> Node::get_all_children() {
    std::vector<std::shared_ptr<Node>> all_children;
    all_children.reserve(embedded_children.size() + children.size());
    all_children.insert(all_children.end(), embedded_children.begin(), embedded_children.end());
    all_children.insert(all_children.end(), children.begin(), children.end());

    return all_children;
}

void Node::add_child(const std::shared_ptr<Node> &new_child) {
    assert(new_child && new_child.get() != this);

    if (std::find(embedded_children.begin(), embedded_children.end(), new_child) != embedded_children.end()) {
        std::cout << "Attempted to add a repated child!" << std::endl;
        return;
    }

    // Set self as the parent of the new node.
    new_child->parent = this;
    new_child->tree_ = tree_;

    children.push_back(new_child);
}

void Node::add_embedded_child(const std::shared_ptr<Node> &new_child) {
    if (std::find(embedded_children.begin(), embedded_children.end(), new_child) != embedded_children.end()) {
        std::cout << "Attempted to add a repeated embedded child!" << std::endl;
        return;
    }

    // Set self as the parent of the new node.
    new_child->parent = this;
    new_child->tree_ = tree_;

    embedded_children.push_back(new_child);
}

std::shared_ptr<Node> Node::get_child(size_t index) {
    if (index > children.size()) {
        return nullptr;
    }

    return children[index];
}

void Node::remove_child(size_t index) {
    if (index < 0 || index >= children.size()) {
        return;
    }
    children.erase(children.begin() + index);
}

void Node::remove_all_children() {
    children.clear();
}

void Node::set_visibility(bool visible) {
    visible_ = visible;
}

bool Node::get_visibility() const {
    return visible_;
}

bool Node::get_global_visibility() const {
    if (parent) {
        return parent->get_global_visibility() && get_visibility();
    }

    return get_visibility();
}

Pathfinder::Window *Node::get_window() const {
    if (type == NodeType::Window) {
        auto sub_window_node = (SubWindow *)this;
        return sub_window_node->get_raw_window().get();
    }

    if (parent) {
        return parent->get_window();
    }

    return RenderServer::get_singleton()->window_builder_->get_primary_window().get();
}

std::string Node::get_node_path() const {
    std::string type_name = ""; // NodeTypeName[static_cast<unsigned __int64>(type)];

    if (parent) {
        return parent->get_node_path() + "/" + type_name;
    }

    return "/" + type_name;
}

void Node::when_parent_size_changed(Vec2F new_size) {
    for (auto &child : children) {
        child->when_parent_size_changed(new_size);
    }
}

void Node::when_subtree_changed() {
    for (auto &callback : subtree_changed_callbacks) {
        callback();
    }

    // Branch->root signal propagation.
    if (parent) {
        parent->when_subtree_changed();
    }
}

void Node::connect_signal(const std::string &signal, const AnyCallable<void> &callback) {
    if (signal == "subtree_changed") {
        subtree_changed_callbacks.push_back(callback);
    }
}

NodeType Node::get_node_type() const {
    return type;
}

SceneTree *Node::get_tree() const {
    return tree_;
}

} // namespace Flint
