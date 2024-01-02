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
    Label,
    TextEdit,
    Panel,
    TextureRect,
    Tree,
    ProgressBar,
    SpinBox,

    Max,
};

class SceneTree;

class Node {
    friend class SceneTree;

public:
    std::string name;

    virtual ~Node() = default;

public:
    virtual void propagate_input(InputEvent &event);

    virtual void propagate_update(double delta);

    virtual void propagate_notify(Signal signal);

    // Override this to intercept the propagation of rendering operations.
    virtual void propagate_draw();

    virtual void propagate_cleanup();

    virtual void input(InputEvent &event);

    virtual void update(double delta);

    virtual void notify(Signal signal);

    virtual void draw();

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

    virtual void set_visibility(bool _visible);

    bool get_visibility() const;

    bool get_global_visibility() const;

    Pathfinder::Window* get_window() const;

    /**
     * Called when the sub-tree structure of this node changed.
     */
    void when_subtree_changed();

    virtual void connect_signal(const std::string &signal, const std::function<void()> &callback);

    void enable_visual_debug(bool enabled);

    SceneTree *get_tree() const;

protected:
    NodeType type = NodeType::Node;

    bool visible = true;

    std::vector<std::shared_ptr<Node>> children;

    // Don't use a shared pointer as it causes circular references.
    // Also, we must initialize it to null.
    Node *parent{};

    SceneTree *tree_;

    // Called when subtree structure changes.
    std::vector<std::function<void()>> subtree_changed_callbacks;

    bool visual_debug = false;
};

} // namespace Flint

#endif // FLINT_NODE_H
