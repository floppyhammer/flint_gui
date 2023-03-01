#include "scene_tree.h"

#include "render/swap_chain.h"
#include "ui_layer.h"
#include "window_proxy.h"

namespace Flint {

SceneTree::SceneTree() {
    root = std::make_shared<WindowProxy>(Vec2I{400, 300}, false);
    root->name = "Main Window";

    auto render_server = RenderServer::get_singleton();
}

void SceneTree::replace_scene(const std::shared_ptr<Node>& new_scene) {
    switch (new_scene->extended_from_which_base_node()) {
        case NodeType::Node:
        case NodeType::Node3d: {
            auto world = std::make_shared<World>(false);
            world->add_child(new_scene);
            root->add_child(world);
        } break;
        case NodeType::Node2d: {
            auto world = std::make_shared<World>(true);
            world->add_child(new_scene);
            root->add_child(world);
        } break;
        case NodeType::NodeUi: {
            auto ui_layer = std::make_shared<UiLayer>();
            ui_layer->add_child(new_scene);
            root->add_child(ui_layer);
        } break;
        default:
            abort();
    }
}

void SceneTree::process(double dt) const {
    if (root == nullptr) {
        return;
    }

    //    if (Window::get_singleton()->framebufferResized) {
    //        auto new_size = Vec2I(Window::get_singleton()->framebuffer_width,
    //        Window::get_singleton()->framebuffer_height); when_window_size_changed(new_size);
    //        VectorServer::get_singleton()->get_canvas()->set_new_dst_texture(new_size);
    //    }

//    root->propagate_input(dt);

    root->propagate_update(dt);

    root->propagate_draw(VK_NULL_HANDLE, VK_NULL_HANDLE);
}

void SceneTree::when_window_size_changed(Vec2I new_size) const {
    for (auto& child : root->get_children()) {
        if (child->get_node_type() == NodeType::UiLayer) {
            auto cast_child = dynamic_cast<UiLayer*>(child.get());
            cast_child->when_window_size_changed(new_size);
        }
    }
}

void SceneTree::quit() {
    quited = true;
}

bool SceneTree::has_quited() const {
    return quited;
}

} // namespace Flint
