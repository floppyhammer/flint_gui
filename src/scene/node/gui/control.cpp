#include "control.h"

#include "../sub_viewport.h"
#include "../../../render/swap_chain.h"
#include "../../../resources/default_resource.h"
#include "../../../common/math/rect.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    Control::Control() {
        type = NodeType::Control;

        outline.bg_color = ColorU();
        outline.corner_radius = 0;
        outline.border_width = 2;
        outline.border_color = ColorU(202, 130, 94, 255);
    }

    Vec2<float> Control::calculate_minimum_size() {
        return minimum_size;
    }

    void Control::draw(VkCommandBuffer p_command_buffer) {
        outline.add_to_canvas(get_global_position(), size, VectorServer::get_singleton().canvas);
    }

    void Control::update(double dt) {
        update_mvp();
    }

    void Control::update_mvp() {
        Node *viewport_node = get_viewport();

        Vec2<uint32_t> viewport_extent;
        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            viewport_extent = viewport->get_extent();
        } else { // Default to swap chain image.
            auto extent = SwapChain::getSingleton().swapChainExtent;
            viewport_extent = Vec2<uint32_t>(extent.width, extent.height);
        }

        // Prepare MVP data. We use this matrix to convert a full-screen to the NodeGui's rect.
        ModelViewProjection mvp{};

        // The actual application order of these matrices is reverse.
        // 4.
        mvp.model = glm::translate(glm::mat4(1.0f),
                                   glm::vec3(position.x / viewport_extent.x * 2.0f,
                                             position.y / viewport_extent.y * 2.0f,
                                             0.0f));
        // 3.
        mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
        // 2.
        mvp.model = glm::scale(mvp.model, glm::vec3(scale.x, scale.y, 1.0f));
        // 1.
        mvp.model = glm::scale(mvp.model,
                               glm::vec3(size.x / viewport_extent.x * 2.0f,
                                         size.y / viewport_extent.y * 2.0f,
                                         1.0f));

        push_constant.model = mvp.model;
    }

    void Control::input(std::vector<InputEvent> &input_queue) {
        if (mouse_filter != MouseFilter::STOP) return;

        auto global_position = get_global_position();

        // Stop mouse input propagation.
        for (auto it = input_queue.begin(); it != input_queue.end();) {
            switch (it->type) {
                case InputEventType::MouseMotion: {
                    if (Rect<float>(global_position, global_position + size).contains_point(it->args.mouse_motion.position)) {
                        it = input_queue.erase(it);
                    } else {
                        ++it;
                    }
                }
                    break;
                case InputEventType::MouseButton: {
                    if (Rect<float>(global_position, global_position + size).contains_point(it->args.mouse_button.position)) {
                        it = input_queue.erase(it);
                    } else {
                        ++it;
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
        size = p_size;
    }

    Vec2<float> Control::get_position() const {
        return position;
    }

    Vec2<float> Control::get_size() const {
        return size;
    }
}
