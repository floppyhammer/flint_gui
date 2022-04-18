#include "node_2d.h"

namespace Flint {
    Node2d::Node2d() {
        type = NodeType::Node2D;

        mvp_buffer = std::make_shared<MvpBuffer>();
    }

    Vec2<float> Node2d::get_global_position() const {
        if (parent != nullptr && parent->extended_from_which_base_node() == NodeType::Node2D) {
            auto cast_parent = dynamic_cast<Node2d *>(parent);

            return cast_parent->get_global_position() + position;
        }

        return position;
    }

    void Node2d::init_default_mesh() {
        mesh = Mesh2D::from_default();
        material = std::make_shared<Material2D>();
    }
}
