#include "texture_rect.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "../../../core/engine.h"
#include "../../../rendering/swap_chain.h"
#include "../sub_viewport.h"

#include <memory>
#include <chrono>

namespace Flint {
    TextureRect::TextureRect() {
        type = NodeType::TextureRect;

        init_default_mesh();
    }

    void TextureRect::set_texture(std::shared_ptr<Texture> p_texture) {
        material->texture = p_texture;

        size.x = (float) p_texture->width;
        size.y = (float) p_texture->height;

        mesh->updateDescriptorSets(material, mvp_buffer->uniform_buffers);
    }

    std::shared_ptr<Texture> TextureRect::get_texture() const {
        return material->texture;
    }

    void TextureRect::_update(double delta) {
        // Update self.
        NodeGui::update(delta);

        // Update children.
        Node::_update(delta);
    }

    void TextureRect::update(double delta) {
    }

    void TextureRect::_draw(VkCommandBuffer p_command_buffer) {
        draw(p_command_buffer);

        Node::_draw(p_command_buffer);
    }

    void TextureRect::draw(VkCommandBuffer p_command_buffer) {
        Node *viewport_node = get_viewport();

        VkPipeline pipeline = RS::getSingleton().blitGraphicsPipeline;

        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            pipeline = viewport->viewport->blitGraphicsPipeline;
        }

        VkBuffer vertexBuffers[] = {mesh->vertexBuffer};
        RS::getSingleton().blit(
                p_command_buffer,
                pipeline,
                mesh->getDescriptorSet(SwapChain::getSingleton().currentImage),
                vertexBuffers,
                mesh->indexBuffer,
                mesh->indices_count);
    }
}
