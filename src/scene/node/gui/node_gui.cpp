#include "node_gui.h"

#include "../sub_viewport.h"
#include "../../../render/swap_chain.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    NodeGui::NodeGui() {
        type = NodeType::NodeGui;

        mvp_buffer = std::make_shared<MvpBuffer>();
    }

    void NodeGui::_update(double delta) {
        // Update self.
        update(delta);

        // Update children.
        Node::_update(delta);
    }

    void NodeGui::update(double delta) {
        update_mvp();
    }

    void NodeGui::update_mvp() {
        Node *viewport_node = get_viewport();

        Vec2<uint32_t> viewport_extent;
        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            viewport_extent = viewport->get_extent();
        } else { // Default to swap chain image.
            auto extent = SwapChain::getSingleton().swapChainExtent;
            viewport_extent = Vec2<uint32_t>(extent.width, extent.height);
        }

        // Prepare UBO data. We use this matrix to convert a full-screen to the NodeGui's rect.
        UniformBufferObject ubo{};

        // The actual application order of these matrices is reverse.
        // 4.
        ubo.model = glm::translate(glm::mat4(1.0f),
                                   glm::vec3(position.x / viewport_extent.x * 2.0f,
                                             position.y / viewport_extent.y * 2.0f,
                                             0.0f));
        // 3.
        ubo.model = glm::translate(ubo.model, glm::vec3(-1.0, -1.0, 0.0f));
        // 2.
        ubo.model = glm::scale(ubo.model, glm::vec3(scale.x, scale.y, 1.0f));
        // 1.
        ubo.model = glm::scale(ubo.model,
                               glm::vec3(size.x / viewport_extent.x * 2.0f,
                                         size.y / viewport_extent.y * 2.0f,
                                         1.0f));

        mvp_buffer->update_uniform_buffer(ubo);
    }

    void NodeGui::init_default_mesh() {
        mesh = Mesh2D::from_default();
        desc_set = std::make_shared<Mesh2dDescSet>();
        material = std::make_shared<Material2D>();
    }
}
