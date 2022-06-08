#include "node_2d.h"

#include "../../../resources/default_resource.h"

namespace Flint {
    Node2d::Node2d() {
        type = NodeType::Node2D;

        mesh = DefaultResource::get_singleton()->new_default_mesh_2d();

        position_indicator.outline.move_to(-12, 0);
        position_indicator.outline.line_to(12, 0);
        position_indicator.outline.move_to(0, -12);
        position_indicator.outline.line_to(0, 12);
        position_indicator.stroke_width = 2;
        position_indicator.stroke_color = ColorU::red();
    }

    Vec2<float> Node2d::get_global_position() const {
        if (parent != nullptr && parent->extended_from_which_base_node() == NodeType::Node2D) {
            auto cast_parent = dynamic_cast<Node2d *>(parent);

            return cast_parent->get_global_position() + position;
        }

        return position;
    }

    void Node2d::draw(VkCommandBuffer p_command_buffer) {
        VectorServer::get_singleton()->draw_path(position_indicator,
                                                 Transform2::from_translation(position));
    }
}
