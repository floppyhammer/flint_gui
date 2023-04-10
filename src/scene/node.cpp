#include "node.h"

#include <string>

#include "../render/swap_chain.h"
#include "window_proxy.h"
#include "world.h"

namespace Flint::Scene {

void Node::propagate_update(double dt) {
    update(dt);

    for (auto &child : children) {
        child->propagate_update(dt);
    }
}

void Node::propagate_draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) {
    draw(render_pass, cmd_buffer);

    for (auto &child : children) {
        child->propagate_draw(render_pass, cmd_buffer);
    }
}

void Node::propagate_notify(Signal signal) {
    notify(signal);

    for (auto &child : children) {
        child->propagate_notify(signal);
    }
}

void Node::propagate_input(InputEvent &event) {
    auto it = children.rbegin();
    while (it != children.rend()) {
        (*it)->propagate_input(event);
        it++;
    }

    input(event);
}

void Node::propagate_cleanup() {
    for (auto &child : children) {
        child->propagate_cleanup();
    }
}

void Node::input(InputEvent &event) {
}

void Node::update(double delta) {
}

void Node::notify(Signal signal) {
}

void Node::draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) {
}

World *Node::get_world() {
    if (parent == nullptr) {
        return nullptr;
    }

    return parent->type == NodeType::World ? (World *)parent : parent->get_world();
}

WindowProxy *Node::get_window() {
    if (parent == nullptr) {
        return nullptr;
    }

    return parent->type == NodeType::Window ? (WindowProxy *)parent : parent->get_window();
}

void Node::set_parent(Node *node) {
    parent = node;
}

Node *Node::get_parent() const {
    return parent;
}

std::vector<std::shared_ptr<Node>> Node::get_children() {
    return children;
}

void Node::add_child(const std::shared_ptr<Node> &new_child) {
    // Set self as the parent of the new node.
    new_child->parent = this;

    //        if (p_child->name.empty()) {
    //            auto node_type_name = NodeTypeName[(uint32_t) p_child->type];
    //
    //            uint32_t same_type_child_count = 0;
    //            for (auto &c: children) {
    //                if (c->type == p_child->type) {
    //                    same_type_child_count++;
    //                }
    //            }
    //
    //            p_child->name = node_type_name + std::to_string(children.size());
    //        }

    children.push_back(new_child);

    //        get_tree()->
}

std::shared_ptr<Node> Node::get_child(size_t index) {
    if (index > children.size()) {
        return nullptr;
    }

    return children[index];
}

void Node::remove_child(size_t index) {
    if (index < 0 || index >= children.size()) return;
    children.erase(children.begin() + index);
}

bool Node::is_ui_node() const {
    return type >= NodeType::NodeUi && type < NodeType::Node2d;
}

void Node::set_visibility(bool _visible) {
    visible = _visible;
}

bool Node::get_visibility() const {
    return visible;
}

bool Node::get_global_visibility() const {
    if (parent) {
        return parent->get_global_visibility() && get_visibility();
    } else {
        return get_visibility();
    }
}

NodeType Node::extended_from_which_base_node() const {
    if (type < NodeType::NodeUi)
        return NodeType::Node;
    else if (type < NodeType::Node2d)
        return NodeType::NodeUi;
    else if (type < NodeType::Node3d)
        return NodeType::Node2d;
    else if (type < NodeType::Max)
        return NodeType::Node3d;
    else
        return NodeType::Max;
}

std::string Node::get_node_path() const {
    std::string type_name = ""; // NodeTypeName[static_cast<unsigned __int64>(type)];

    if (parent) {
        return parent->get_node_path() + "/" + type_name;
    } else {
        return "/" + type_name;
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

void Node::connect_signal(const std::string &signal, const std::function<void()> &callback) {
    if (signal == "subtree_changed") {
        subtree_changed_callbacks.push_back(callback);
    }
}

void Node::enable_visual_debug(bool enabled) {
    visual_debug = enabled;
}

NodeType Node::get_node_type() const {
    return type;
}

uint32_t Node::get_current_image() {
    WindowProxy *window = get_window();
    if (!window) {
        abort();
    }

    uint32_t current_image = window->swapchain->currentImage;

    return current_image;
}

} // namespace Flint::Scene
