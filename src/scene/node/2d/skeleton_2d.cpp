#include "skeleton_2d.h"

#include "../sub_viewport.h"
#include "../../../render/mvp_buffer.h"
#include "../../../render/swap_chain.h"
#include "../../../servers/vector_server.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Flint {
    void Bone2d::add_child(std::shared_ptr<Bone2d> child) {
        children.push_back(child);
        child->parent = this;
    }

    void Bone2d::draw() {
        auto global_position = get_global_position();

        VShape vshape_start;

        // Bone starting point.
        vshape_start.shape.add_circle({0, 0}, 5);
        vshape_start.stroke_color = ColorU(200, 200, 200, 255);
        vshape_start.stroke_width = 3;

        Transform2 start_transform = Transform2::from_translation(global_position);

        VectorServer::get_singleton()->draw_vshape(vshape_start, start_transform);

        // Draw bone to parent connection.
        if (parent) {
            // Draw in global coordinates.
            auto parent_global_position = parent->get_global_position();
            auto real_length = (global_position - parent_global_position).length();

            // If the bone is too short, don't draw the bone body.
            if (real_length > 3) {
                VShape vshape;
                vshape.shape.move_to(6, 0);
                vshape.shape.line_to(12, -6);
                vshape.shape.line_to(real_length, 0);
                vshape.shape.line_to(12, 6);
                vshape.shape.close();

                vshape.fill_color = ColorU(200, 200, 200, 255);

                VectorServer::get_singleton()->draw_vshape(vshape, start_transform);
            }
        }

        // Draw terminal bone.
        if (children.empty()) {
            // If the bone is too short, don't draw the bone body.
            if (length > 3) {
                VShape vshape;
                vshape.shape.move_to(6, 0);
                vshape.shape.line_to(12, -6);
                vshape.shape.line_to(length, 0);
                vshape.shape.line_to(12, 6);
                vshape.shape.close();

                vshape.fill_color = ColorU(200, 200, 200, 255);

                VectorServer::get_singleton()->draw_vshape(vshape, start_transform);
            }
        } else {
            for (auto &bone: children) {
                bone->draw();
            }
        }
    }

    Vec2F Bone2d::get_global_position() {
        if (parent) {
            return position + parent->get_global_position();
        }

        if (skeleton) {
            return position + skeleton->get_global_position();
        }

        // TODO: Should also consider the skeleton's global position.
        return position;
    }

    float Bone2d::get_global_rotation() {
        if (parent) {
            return parent->get_global_rotation();
        }

        // TODO: Should also consider the skeleton's global rotation.
        return rotation;
    }

    Skeleton2d::Skeleton2d() {
        type = NodeType::Skeleton2D;

        base_bone = std::make_shared<Bone2d>();
        base_bone->skeleton = this;

        auto neck_bone = std::make_shared<Bone2d>();
        neck_bone->position = {0, -4};
        neck_bone->length = 4;
        base_bone->add_child(neck_bone);

        auto head_bone = std::make_shared<Bone2d>();
        head_bone->position = {0, -20};
        head_bone->length = 20;
        neck_bone->add_child(head_bone);

        auto left_shoulder_bone = std::make_shared<Bone2d>();
        left_shoulder_bone->position = {-50, 10};
        left_shoulder_bone->length = 50;
        base_bone->add_child(left_shoulder_bone);

        auto left_arm_bone = std::make_shared<Bone2d>();
        left_arm_bone->position = {-50, 10};
        left_arm_bone->length = 50;
        left_shoulder_bone->add_child(left_arm_bone);

        auto right_shoulder_bone = std::make_shared<Bone2d>();
        right_shoulder_bone->position = {50, 10};
        right_shoulder_bone->length = 50;
        base_bone->add_child(right_shoulder_bone);

        auto right_arm_bone = std::make_shared<Bone2d>();
        right_arm_bone->position = {50, 10};
        right_arm_bone->length = 50;
        right_shoulder_bone->add_child(right_arm_bone);

        auto left_hip_bone = std::make_shared<Bone2d>();
        left_hip_bone->position = {-40, 100};
        left_hip_bone->length = 50;
        base_bone->add_child(left_hip_bone);

        auto left_leg_bone = std::make_shared<Bone2d>();
        left_leg_bone->position = {0, 60};
        left_leg_bone->length = 50;
        left_hip_bone->add_child(left_leg_bone);

        auto left_foot_bone = std::make_shared<Bone2d>();
        left_foot_bone->position = {0, 60};
        left_foot_bone->length = 50;
        left_leg_bone->add_child(left_foot_bone);

        auto right_hip_bone = std::make_shared<Bone2d>();
        right_hip_bone->position = {40, 100};
        right_hip_bone->length = 50;
        base_bone->add_child(right_hip_bone);

        auto right_leg_bone = std::make_shared<Bone2d>();
        right_leg_bone->position = {0, 60};
        right_leg_bone->length = 50;
        right_hip_bone->add_child(right_leg_bone);

        auto right_foot_bone = std::make_shared<Bone2d>();
        right_foot_bone->position = {0, 60};
        right_foot_bone->length = 50;
        right_leg_bone->add_child(right_foot_bone);
    }

    void Skeleton2d::set_texture(std::shared_ptr<ImageTexture> p_texture) {
        mesh->surface->get_material()->set_texture(p_texture);
    }

    std::shared_ptr<ImageTexture> Skeleton2d::get_texture() const {
        return mesh->surface->get_material()->get_texture();
    }

    void Skeleton2d::set_mesh(const std::shared_ptr<Mesh2d> &p_mesh) {
        mesh = p_mesh;
    }

    void Skeleton2d::set_material(const std::shared_ptr<Material2d> &p_material) {
        mesh->surface->set_material(p_material);
    }

    void Skeleton2d::update(double delta) {
    }

    void Skeleton2d::update_mvp() {
        if (mesh == nullptr || mesh->surface == nullptr) {
            return;
        }

        Node *viewport_node = get_viewport();

        Vec2<uint32_t> viewport_extent;
        if (viewport_node) {
            auto viewport = dynamic_cast<SubViewport *>(viewport_node);
            viewport_extent = viewport->get_extent();
        } else { // Default to swap chain image.
            auto extent = SwapChain::getSingleton()->swapChainExtent;
            viewport_extent = Vec2<uint32_t>(extent.width, extent.height);
        }

        float sprite_width = mesh->surface->get_material()->get_texture()->get_width() * scale.x;
        float sprite_height = mesh->surface->get_material()->get_texture()->get_height() * scale.y;

        auto global_position = get_global_position();

        // Prepare MVP data. We use this matrix to convert a full-screen to the control's rect.
        ModelViewProjection mvp{};

        // The actual application order of these matrices is reverse.
        // 4.
        mvp.model = glm::translate(glm::mat4(1.0f),
                                   glm::vec3(global_position.x / viewport_extent.x * 2.0f,
                                             global_position.y / viewport_extent.y * 2.0f,
                                             0.0f));
        // 3.
        mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
        // 2.
        mvp.model = glm::scale(mvp.model, glm::vec3(scale.x, scale.y, 1.0f));
        // 1.
        mvp.model = glm::scale(mvp.model,
                               glm::vec3(sprite_width / viewport_extent.x * 2.0f,
                                         sprite_height / viewport_extent.y * 2.0f,
                                         1.0f));

        push_constant.model = mvp.model;
    }

    void Skeleton2d::draw(VkCommandBuffer p_command_buffer) {
        if (base_bone) {
            base_bone->draw();
        }
    }
}
