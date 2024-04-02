#ifndef FLINT_NODE_H
#define FLINT_NODE_H

#include <memory>
#include <vector>

#include "../common/utils.h"
#include "../servers/engine.h"
#include "../servers/input_server.h"

namespace Flint {

enum class NodeType {
    // General.
    Node = 0,
    Window,

    // UI.
    NodeUi,

    Container,
    CenterContainer,
    MarginContainer,
    HStackContainer,
    VStackContainer,
    ScrollContainer,
    TabContainer,

    Button,
    MenuButton,   // todo
    OptionButton, // todo
    CheckButton,  // todo
    RadioButton,  // todo

    Label,
    TextEdit,
    SpinBox,
    Panel,
    TextureRect,
    Tree,
    ProgressBar,
    PopupMenu,

    NotInstantiable,

    Max,
};

std::string get_node_type_name(NodeType type);

class SceneTree;

/// Position-independent, window-independent base node.
class Node {
    friend class SceneTree;

public:
    std::string name;

    virtual ~Node() = default;

    virtual void propagate_input(InputEvent &event);

    virtual void propagate_update(double delta);

    virtual void propagate_notify(Signal signal);

    // Override this to intercept the propagation of rendering operations.
    virtual void propagate_draw();

    virtual void propagate_cleanup();

    virtual void input(InputEvent &event);

    virtual void update(double dt);

    virtual void notify(Signal signal);

    virtual void draw();

    virtual void custom_update(double dt) {
    }

    virtual void custom_input(InputEvent &event) {
    }

    void add_child(const std::shared_ptr<Node> &new_child);

    NodeType get_node_type() const;

    void set_parent(Node *node);

    Node *get_parent() const;

    std::vector<std::shared_ptr<Node>> get_children();

    virtual std::shared_ptr<Node> get_child(size_t index);

    void remove_child(size_t index);

    virtual bool is_ui_node() const {
        return false;
    }

    std::string get_node_path() const;

    virtual void set_visibility(bool visible);

    bool get_visibility() const;

    bool get_global_visibility() const;

    Pathfinder::Window *get_window() const;

    virtual void when_parent_size_changed(Vec2F new_size);

    /**
     * Called when the sub-tree structure of this node changed.
     */
    void when_subtree_changed();

    virtual void connect_signal(const std::string &signal, const std::function<void()> &callback);

    SceneTree *get_tree() const;

protected:
    NodeType type = NodeType::Node;

    bool visible_ = true;

    std::vector<std::shared_ptr<Node>> children;

    // Don't use a shared pointer as it causes circular references.
    // Also, we must initialize it to null.
    Node *parent{};

    SceneTree *tree_;

    // Called when subtree structure changes.
    std::vector<std::function<void()>> subtree_changed_callbacks;
};

/// Return depth first search postorder traversal of the scene tree.
/// Nodes on the same level will be returned from left to right.
/// Major usages: container layout adjustment.
void dfs_postorder_traversal(Node *node, std::vector<Node *> &ordered_nodes);

} // namespace Flint

#endif // FLINT_NODE_H
