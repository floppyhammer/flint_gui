#include "texture_rect.h"

#include <memory>

#include "../../common/logger.h"
#include "../../render/swap_chain.h"
#include "../../resources/default_resource.h"
#include "../../resources/image_texture.h"
#include "../../resources/vector_texture.h"
#include "../../servers/core_server.h"
#include "../sub_viewport.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {

TextureRect::TextureRect() {
    type = NodeType::TextureRect;
}

void TextureRect::set_texture(const std::shared_ptr<Texture> &p_texture) {
    // Texture can be null.
    texture = p_texture;

    if (!texture) {
        return;
    }

    if (texture->get_type() == TextureType::IMAGE) {
        mesh = DefaultResource::get_singleton()->new_default_mesh_2d();
        auto image_texture = dynamic_cast<ImageTexture *>(texture.get());
        mesh->surface->get_material()->set_texture(image_texture);
    }
}

std::shared_ptr<Texture> TextureRect::get_texture() const {
    return texture;
}

void TextureRect::update(double dt) {
    Control::update(dt);

    update_mvp();
}

void TextureRect::draw(VkCommandBuffer p_command_buffer) {
    if (texture) {
        // Image texture.
        // TODO: draw image by Pathfinder.
        if (texture->get_type() == TextureType::IMAGE) {
            Node *viewport_node = get_viewport();

            VkPipeline pipeline = RenderServer::getSingleton()->blitGraphicsPipeline;
            VkPipelineLayout pipeline_layout = RenderServer::getSingleton()->blitPipelineLayout;

            if (viewport_node) {
                auto viewport = dynamic_cast<SubViewport *>(viewport_node);
                pipeline = viewport->render_target->blitGraphicsPipeline;
            }

            // Upload the model matrix to the GPU via push constants.
            vkCmdPushConstants(p_command_buffer,
                               pipeline_layout,
                               VK_SHADER_STAGE_VERTEX_BIT,
                               0,
                               sizeof(MvpPushConstant),
                               &push_constant);

            // Unlike Sprite 2D, Texture Rect should not support custom mesh.
            RenderServer::getSingleton()->blit(p_command_buffer,
                                               pipeline,
                                               mesh->surface->get_material()->get_desc_set()->getDescriptorSet(
                                                   SwapChain::getSingleton()->currentImage));
        } else { // Vector texture.
            auto vector_server = VectorServer::get_singleton();

            auto vector_texture = static_cast<VectorTexture *>(texture.get());

            auto global_position = get_global_position();

            auto texture_size = vector_texture->get_size().to_f32();

            Pathfinder::Transform2 transform;

            switch (stretch_mode) {
                case StretchMode::KeepCentered: {
                    auto offset = (size - texture_size) * 0.5f;

                    transform = Transform2::from_translation(global_position + offset);
                } break;
                case StretchMode::Scale: {
                    if (texture_size.area() == 0) {
                        Logger::error("Vector texture size is invalid!", "TextureRect");
                        return;
                    }
                    auto scale = size / vector_texture->get_size().to_f32();

                    transform = Transform2::from_scale(scale).translate(global_position);
                } break;
                case StretchMode::KeepAspect:
                case StretchMode::KeepAspectCentered: {
                    if (texture_size.area() == 0) {
                        Logger::error("Vector texture size is invalid!", "TextureRect");
                        return;
                    }

                    auto tex_aspect_ratio =
                        vector_texture->get_size().to_f32().x / vector_texture->get_size().to_f32().y;
                    auto rect_aspect_ratio = size.x / size.y;

                    Vec2F scale;
                    if (tex_aspect_ratio > rect_aspect_ratio) {
                        scale = Vec2F(size.x / vector_texture->get_size().x);
                    } else {
                        scale = Vec2F(size.y / vector_texture->get_size().y);
                    }

                    Vec2F effective_size = vector_texture->get_size().to_f32() * scale;

                    Vec2F offset;
                    if (stretch_mode == StretchMode::KeepAspectCentered) {
                        offset = (size - effective_size) * 0.5f;
                    }

                    transform = Transform2::from_scale(scale).translate(global_position + offset);
                } break;
                default: {
                    transform = Transform2::from_translation(global_position);
                } break;
            }

            vector_server->draw_vector_texture(*vector_texture, transform);
        }
    }

    Control::draw(p_command_buffer);
}

void TextureRect::update_mvp() {
    Vec2I viewport_extent = get_viewport_size();

    // Prepare MVP data. We use this matrix to convert a full-screen to the NodeGui's rect.
    ModelViewProjection mvp{};

    // The actual application order of these matrices is reverse.
    // 4.
    mvp.model = glm::translate(
        glm::mat4(1.0f), glm::vec3(position.x / viewport_extent.x * 2.0f, position.y / viewport_extent.y * 2.0f, 0.0f));
    // 3.
    mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
    // 2.
    mvp.model = glm::scale(mvp.model, glm::vec3(scale.x, scale.y, 1.0f));
    // 1.
    mvp.model =
        glm::scale(mvp.model, glm::vec3(size.x / viewport_extent.x * 2.0f, size.y / viewport_extent.y * 2.0f, 1.0f));

    push_constant.mvp = mvp.model;
}

Vec2F TextureRect::calc_minimum_size() const {
    return minimum_size.max(texture ? Vec2F(texture->get_width(), texture->get_height()) : Vec2<float>(0));
}

void TextureRect::set_stretch_mode(TextureRect::StretchMode new_stretch_mode) {
    stretch_mode = new_stretch_mode;
}

} // namespace Flint
