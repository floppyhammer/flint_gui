#ifndef FLINT_NODE_H
#define FLINT_NODE_H

#include <memory>
#include <vector>

#include "../servers/core_server.h"
#include "../servers/input_server.h"
#include "common/logger.h"
#include "servers/render_server.h"

namespace Flint {

enum class NodeType {
    // General.
    Node = 0,
    World,
    UiLayer,

    // UI.
    NodeUi,
    Container,
    CenterContainer,
    MarginContainer,
    HStackContainer,
    VStackContainer,
    ScrollContainer,
    Window,
    Popup,
    TabContainer,
    Button,
    Label,
    TextEdit,
    Panel,
    TextureRect,
    Tree,
    ProgressBar,
    SpinBox,

    // 2D.
    Node2d,
    Sprite2d,
    Skeleton2d,

    // 3D.
    Node3d,
    Sprite3d,
    Model,
    Skybox,

    Max,
};

class World;
class WindowProxy;

class Node {
public:
    std::string name;

public:
    virtual void propagate_input(InputEvent &event);

    virtual void propagate_update(double delta);

    virtual void propagate_notify(Signal signal);

    // Override this to intercept the propagation of rendering operations.
    virtual void propagate_draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer);

    virtual void propagate_cleanup();

    virtual void input(InputEvent &event);

    virtual void update(double delta);

    virtual void notify(Signal signal);

    virtual void draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer);

    void add_child(const std::shared_ptr<Node> &new_child);

    NodeType get_node_type() const;

    /**
     * Get the viewport this node belongs to.
     * @return A pointer to the viewport.
     */
    World *get_world();

    WindowProxy *get_window();

    uint32_t get_current_image();

    void set_parent(Node *node);

    Node *get_parent() const;

    std::vector<std::shared_ptr<Node>> get_children();

    virtual std::shared_ptr<Node> get_child(size_t index);

    void remove_child(size_t index);

    NodeType extended_from_which_base_node() const;

    bool is_ui_node() const;

    std::string get_node_path() const;

    void set_visibility(bool _visible);

    bool get_visibility() const;

    bool get_global_visibility() const;

    /**
     * Called when the sub-tree structure of this node changed.
     */
    void when_subtree_changed();

    virtual void connect_signal(const std::string &signal, const std::function<void()> &callback);

    void enable_visual_debug(bool enabled);

protected:
    NodeType type = NodeType::Node;

    bool visible = true;

    std::vector<std::shared_ptr<Node>> children;

    // Don't use a shared pointer as it causes circular references.
    // Also, we must initialize it to null.
    Node *parent{};

    // Called when subtree structure changes.
    std::vector<std::function<void()>> subtree_changed_callbacks;

    bool visual_debug = false;
};

} // namespace Flint

#endif // FLINT_NODE_H
