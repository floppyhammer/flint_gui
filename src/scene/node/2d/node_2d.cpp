#include "node_2d.h"

#include "../../../resources/default_resource.h"

namespace Flint {
    Node2d::Node2d() {
        type = NodeType::Node2D;

        mesh = DefaultResource::get_singleton().default_mesh_2d;
        material = std::make_shared<Material2d>();
        desc_set = std::make_shared<Mesh2dDescSet>();
    }

    Vec2<float> Node2d::get_global_position() const {
        if (parent != nullptr && parent->extended_from_which_base_node() == NodeType::Node2D) {
            auto cast_parent = dynamic_cast<Node2d *>(parent);

            return cast_parent->get_global_position() + position;
        }

        return position;
    }
}
