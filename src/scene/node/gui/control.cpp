#include "control.h"

#include "../sub_viewport.h"
#include "../../../render/swap_chain.h"
#include "../../../resources/default_resource.h"
#include "../../../common/math/rect.h"

namespace Flint {
    Control::Control() {
        type = NodeType::Control;

        outline.bg_color = ColorU();
        outline.corner_radius = 0;
        outline.border_width = 0;
        outline.border_color = ColorU(202, 130, 94, 255);
    }

    Vec2<float> Control::calculate_minimum_size() {
        return minimum_size;
    }

    void Control::draw(VkCommandBuffer p_command_buffer) {
        if (size.x > 0 && size.y > 0)
            outline.add_to_canvas(get_global_position(), size, VectorServer::get_singleton().canvas);
    }

    void Control::update(double dt) {

    }

    void Control::input(std::vector<InputEvent> &input_queue) {
        if (mouse_filter != MouseFilter::STOP) return;

        auto global_position = get_global_position();

        // Handle mouse input propagation.
        for (auto &event : input_queue) {
            switch (event.type) {
                case InputEventType::MouseMotion: {
                    if (Rect<float>(global_position, global_position + size).contains_point(event.args.mouse_motion.position)) {
                        event.consume();
                    }
                }
                    break;
                case InputEventType::MouseButton: {
                    if (Rect<float>(global_position, global_position + size).contains_point(event.args.mouse_button.position)) {
                        event.consume();
                    }
                }
                    break;
                default:
                    break;
            }
        }
    }

    Vec2<float> Control::get_global_position() const {
        if (parent != nullptr && parent->extended_from_which_base_node() == NodeType::Control) {
            auto cast_parent = dynamic_cast<Control *>(parent);

            return cast_parent->get_global_position() + position;
        }

        return position;
    }

    void Control::set_mouse_filter(MouseFilter filter) {
        mouse_filter = filter;
    }

    void Control::set_position(Vec2<float> p_position) {
        position = p_position;
    }

    void Control::set_size(Vec2<float> p_size) {
        if (size == p_size) return;

        size = p_size.max(calculate_minimum_size());
    }

    Vec2<float> Control::get_position() const {
        return position;
    }

    Vec2<float> Control::get_size() const {
        return size;
    }
}
