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

    Transform2 Bone2d::get_transform() {
        Transform2 transform;
        transform.vector = position;

        // Local rotation in Cartesian coordinates.
        float cartesian_rotation = rotation;
//        if (parent) {
//            auto normalized = position / position.length();
//            cartesian_rotation += atan2(normalized.y, normalized.x);
//        }

        transform = transform.rotate(cartesian_rotation);
        return transform;
    }

    Transform2 Bone2d::get_global_transform() {
        if (parent) {
            return parent->get_global_transform() * get_transform();
        } else {
            if (skeleton) {
                return Transform2::from_translation(skeleton->get_global_position()) * get_transform();
            } else {
                return get_transform();
            }
        }
    }

    void Bone2d::draw() {
        VShape vshape_start;

        // Bone starting point.
        vshape_start.shape.add_circle({0, 0}, 5);
        vshape_start.stroke_color = ColorU(200, 200, 200, 255);
        vshape_start.stroke_width = 3;

        Transform2 start_transform = get_global_transform();
        VectorServer::get_singleton()->draw_vshape(vshape_start, start_transform);

        // Draw bone to parent connection.
        if (parent) {
            // Draw in global coordinates.
            auto parent_global_transform = parent->get_global_transform();
            auto distance_to_parent = (start_transform.vector - parent_global_transform.vector).length();

            // If the bone is too short, don't draw the bone body.
            if (distance_to_parent > 3) {
                VShape vshape;
                vshape.shape.move_to(6, 0);
                vshape.shape.line_to(12, -6);
                vshape.shape.line_to(distance_to_parent, 0);
                vshape.shape.line_to(12, 6);
                vshape.shape.close();
                vshape.fill_color = ColorU(200, 200, 200, 255);
                vshape.stroke_color = ColorU(0, 0, 0, 150);
                vshape.stroke_width = 1;

                // Local rotation in Cartesian coordinates.
                float cartesian_rotation = rotation;
                if (parent) {
                    auto normalized = position / position.length();
                    cartesian_rotation += atan2(normalized.y, normalized.x);
                }
                auto rot_transform = Transform2::from_rotation(cartesian_rotation);
                VectorServer::get_singleton()->draw_vshape(vshape, parent_global_transform * rot_transform);
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
                vshape.stroke_color = ColorU(0, 0, 0, 150);
                vshape.stroke_width = 1;

                VectorServer::get_singleton()->draw_vshape(vshape, start_transform);
            }
        } else {
            for (auto &bone: children) {
                bone->draw();
            }
        }
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

    void Skeleton2d::update(double delta) {
    }

    void Skeleton2d::draw(VkCommandBuffer p_command_buffer) {
        if (base_bone) {
            base_bone->draw();
        }
    }
}
