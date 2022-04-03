#include "control.h"

#include "../sub_viewport.h"
#include "../../../rendering/swap_chain.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    Control::Control() {
        type = NodeType::Control;

        mvp_buffer = std::make_shared<MvpBuffer>();
    }

    void Control::set_rect_position(float x, float y) {
        rect_position.x = x;
        rect_position.y = y;
    }

    void Control::set_rect_size(float w, float h) {
        rect_size.x = w;
        rect_size.y = h;
    }

    void Control::set_rect_scale(float x, float y) {
        rect_scale.x = x;
        rect_scale.y = y;
    }

    void Control::set_rect_rotation(float r) {
        rect_rotation = r;
    }

    void Control::set_rect_pivot_offset(float x, float y) {
        rect_pivot_offset.x = x;
        rect_pivot_offset.y = y;
    }

    void Control::_update(double delta) {
        // Root to leaf.
        update(delta);

        Node::_update(delta);
    }

    void Control::update(double delta) {
        update_mvp();
    }

    void Control::update_mvp() {
        Node *viewport_node = get_viewport();

        Vec2<uint32_t> viewport_extent;
        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            viewport_extent = viewport->extent;
        } else { // Default to swap chain image.
            auto extent = SwapChain::getSingleton().swapChainExtent;
            viewport_extent = Vec2<uint32_t>(extent.width, extent.height);
        }

        // Prepare UBO data. We use this matrix to convert a full-screen to the control's rect.
        UniformBufferObject ubo{};

        // The actual application order of these matrices is reverse.
        // 4.
        ubo.model = glm::translate(glm::mat4(1.0f),
                                   glm::vec3(rect_position.x / viewport_extent.x * 2.0f,
                                             rect_position.y / viewport_extent.y * 2.0f,
                                             0.0f));
        // 3.
        ubo.model = glm::translate(ubo.model, glm::vec3(-1.0, -1.0, 0.0f));
        // 2.
        ubo.model = glm::scale(ubo.model, glm::vec3(rect_scale.x, rect_scale.y, 1.0f));
        // 1.
        ubo.model = glm::scale(ubo.model,
                               glm::vec3(rect_size.x / viewport_extent.x * 2.0f,
                                         rect_size.y / viewport_extent.y * 2.0f,
                                         1.0f));

        mvp_buffer->update_uniform_buffer(ubo);
    }

    void Control::init_default_mesh() {
        mesh = Mesh2D::from_default();
        material = std::make_shared<Material2D>();
    }

    Vec2<float> Control::get_rect_position() const {
        return rect_position;
    }

    Vec2<float> Control::get_set_rect_size() const {
        return rect_size;
    }

    Vec2<float> Control::get_rect_scale() const {
        return rect_scale;
    }

    float Control::get_rect_rotation() const {
        return rect_rotation;
    }

    Vec2<float> Control::get_rect_pivot_offset() const {
        return rect_pivot_offset;
    }
}
