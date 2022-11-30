#include "skeleton_2d.h"

#include "../../render/mvp.h"
#include "../../render/render_server.h"
#include "../../render/swap_chain.h"
#include "../../resources/resource_manager.h"
#include "../../common/logger.h"
#include "../sub_viewport.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "poly2tri/poly2tri.h"

namespace Flint {
void Bone2d::add_child(const std::shared_ptr<Bone2d> &child) {
    children.push_back(child);
    child->parent = this;
}

Transform2 Bone2d::get_transform() {
    Transform2 transform;

    // First do rotation, then do translation.

    // 1. Rotation.
    // Local rotation around the bone's start-point.
    transform = transform.rotate(rotation);

    // 2. Translation.
    // Position of the bone's start-point .
    transform = transform.translate(position);

    return transform;
}

Transform2 Bone2d::get_rest_transform() {
    // Bone's start-point position.
    auto transform = Transform2::from_translation(position);

    return transform;
}

Transform2 Bone2d::get_global_transform() {
    if (parent) {
        return parent->get_global_transform() * get_transform();
    } else {
        return get_transform();
    }
}

Transform2 Bone2d::get_global_rest_transform() {
    if (parent) {
        return parent->get_global_rest_transform() * get_rest_transform();
    } else {
        return get_rest_transform();
    }
}

VectorPath get_bone_body_path(float head_length, float bone_length) {
    VectorPath vp_body;
    vp_body.path2d.move_to(head_length, 0);
    vp_body.path2d.line_to(head_length * 2, -head_length);
    vp_body.path2d.line_to(bone_length, 0);
    vp_body.path2d.line_to(head_length * 2, head_length);
    vp_body.path2d.close_path();
    vp_body.fill_color = ColorU(200, 200, 200, 255);
    vp_body.stroke_color = ColorU(0, 0, 0, 150);
    vp_body.stroke_width = 1;
    return vp_body;
}

void Bone2d::draw() {
    auto skeleton_transform = Transform2::from_translation(get_skeleton()->get_global_position());

    VectorPath vp_head;

    // Bone starting point.
    vp_head.path2d.add_circle({0, 0}, 5);
    vp_head.stroke_color = ColorU(200, 200, 200, 255);
    vp_head.stroke_width = 3;

    Transform2 global_transform = get_global_transform();

    // Draw bone head.
    VectorServer::get_singleton()->draw_path(vp_head, skeleton_transform * global_transform);

    float arrow_head_length = 6;

    // Draw terminal bone.
    if (children.empty()) {
        // If the bone is too short, don't draw the bone body.
        if (length > arrow_head_length) {
            auto vp_body = get_bone_body_path(arrow_head_length, length);

            VectorServer::get_singleton()->draw_path(vp_body, skeleton_transform * global_transform);
        }
    } else {
        for (auto &child : children) {
            // (global_transform.get_position() - bone->get_global_transform().get_position()).length();
            auto child_length = child->position.length();

            // If the bone is too short, don't draw the bone body.
            if (child_length > arrow_head_length) {
                auto vp_body = get_bone_body_path(arrow_head_length, child_length);

                auto unit_vector = child->position / child_length;
                float child_rotation = atan2(unit_vector.y, unit_vector.x);

                auto local_rotation_transform = Transform2::from_rotation(child_rotation);
                VectorServer::get_singleton()->draw_path(
                    vp_body, skeleton_transform * global_transform * local_rotation_transform);
            }

            child->draw();
        }
    }
}

Skeleton2d *Bone2d::get_skeleton() {
    if (skeleton) {
        return skeleton;
    } else {
        return parent->get_skeleton();
    }
}

Skeleton2d::Skeleton2d() {
    type = NodeType::Skeleton2D;

    sprite = std::make_shared<Sprite2d>();
    sprite->set_texture(ResourceManager::get_singleton()->load<ImageTexture>("../assets/skeleton_2d_texture.png"));
    sprite->set_parent(this);

    root_bone = std::make_shared<Bone2d>();
    root_bone->skeleton = this;
    bones.push_back(root_bone.get());

    auto neck_bone = std::make_shared<Bone2d>();
    neck_bone->position = {0, 0};
    root_bone->add_child(neck_bone);
    bones.push_back(neck_bone.get());

    auto head_bone = std::make_shared<Bone2d>();
    head_bone->position = {0, -42.5};
    head_bone->length = 20;
    neck_bone->add_child(head_bone);
    bones.push_back(head_bone.get());

    auto left_shoulder_bone = std::make_shared<Bone2d>();
    left_shoulder_bone->position = {-29, 3};
    root_bone->add_child(left_shoulder_bone);
    bones.push_back(left_shoulder_bone.get());

    auto left_arm_bone = std::make_shared<Bone2d>();
    left_arm_bone->position = {-39.5, 2.6};
    left_shoulder_bone->add_child(left_arm_bone);
    bones.push_back(left_arm_bone.get());

    auto left_hand_bone = std::make_shared<Bone2d>();
    left_hand_bone->position = {-47.3, 1.8};
    left_hand_bone->length = 25;
    left_arm_bone->add_child(left_hand_bone);
    bones.push_back(left_hand_bone.get());

    auto right_shoulder_bone = std::make_shared<Bone2d>();
    right_shoulder_bone->position = {29, 3};
    root_bone->add_child(right_shoulder_bone);
    bones.push_back(right_shoulder_bone.get());

    auto right_arm_bone = std::make_shared<Bone2d>();
    right_arm_bone->position = {39.5, 2.6};
    right_shoulder_bone->add_child(right_arm_bone);
    bones.push_back(right_arm_bone.get());

    auto right_hand_bone = std::make_shared<Bone2d>();
    right_hand_bone->position = {47.3, 1.8};
    right_hand_bone->length = 25;
    right_arm_bone->add_child(right_hand_bone);
    bones.push_back(right_hand_bone.get());

    auto left_hip_bone = std::make_shared<Bone2d>();
    left_hip_bone->position = {-14.5, 100};
    root_bone->add_child(left_hip_bone);
    bones.push_back(left_hip_bone.get());

    auto left_leg_bone = std::make_shared<Bone2d>();
    left_leg_bone->position = {-10.3, 68};
    left_hip_bone->add_child(left_leg_bone);
    bones.push_back(left_leg_bone.get());

    auto left_foot_bone = std::make_shared<Bone2d>();
    left_foot_bone->position = {-7.6, 66.1};
    left_foot_bone->length = 50;
    left_leg_bone->add_child(left_foot_bone);
    bones.push_back(left_foot_bone.get());

    auto right_hip_bone = std::make_shared<Bone2d>();
    right_hip_bone->position = {16, 97.5};
    root_bone->add_child(right_hip_bone);
    bones.push_back(right_hip_bone.get());

    auto right_leg_bone = std::make_shared<Bone2d>();
    right_leg_bone->position = {9.3, 70.1};
    right_hip_bone->add_child(right_leg_bone);
    bones.push_back(right_leg_bone.get());

    auto right_foot_bone = std::make_shared<Bone2d>();
    right_foot_bone->position = {7.6, 68};
    right_foot_bone->length = 50;
    right_leg_bone->add_child(right_foot_bone);
    bones.push_back(right_foot_bone.get());

    mesh = std::make_shared<Skeleton2dMesh>();

    // Mesh.
    // Root bone as the origin.
    mesh->vertexes = {
        Vec2F(19.5, -58.1),  Vec2F(19.7, -24.0),  Vec2F(14.1, -19.4),  Vec2F(13.8, -13.6),  Vec2F(17.6, -11.8),
        Vec2F(24.9, -10.1),  Vec2F(29.4, -9.1),   Vec2F(33.5, -8.3),   Vec2F(62.3, -4.2),   Vec2F(66.4, -3.9),
        Vec2F(70.4, -3.5),   Vec2F(149.3, 2.0),   Vec2F(147.3, 26.6),  Vec2F(70.1, 21.2),   Vec2F(64.6, 20.8),
        Vec2F(59.3, 20.5),   Vec2F(35.2, 20.5),   Vec2F(33.5, 83.9),   Vec2F(34.3, 91.2),   Vec2F(35.5, 98.6),
        Vec2F(38.9, 155.6),  Vec2F(39.8, 163.3),  Vec2F(40.8, 169.5),  Vec2F(53.2, 248.1),  Vec2F(24.5, 253.0),
        Vec2F(12.7, 175.6),  Vec2F(11.5, 168.7),  Vec2F(9.9, 160.7),   Vec2F(1.4, 124.9),   Vec2F(-8.0, 159.2),
        Vec2F(-9.7, 166.6),  Vec2F(-11.7, 174.4), Vec2F(-25.7, 252.6), Vec2F(-53.4, 247.0), Vec2F(-40.5, 170.8),
        Vec2F(-39.7, 163.3), Vec2F(-38.9, 155.7), Vec2F(-33.6, 100.0), Vec2F(-33.2, 92.3),  Vec2F(-32.7, 84.0),
        Vec2F(-34.8, 21.0),  Vec2F(-62.3, 21.0),  Vec2F(-67.7, 21.2),  Vec2F(-73.8, 21.4),  Vec2F(-147.2, 24.8),
        Vec2F(-148.4, 0.5),  Vec2F(-75.1, -2.6),  Vec2F(-69.2, -3.1),  Vec2F(-63.1, -3.6),  Vec2F(-36.1, -6.7),
        Vec2F(-30.1, -7.3),  Vec2F(-24.5, -8.3),  Vec2F(-16.7, -12.0), Vec2F(-13.0, -14.4), Vec2F(-13.0, -19.8),
        Vec2F(-19.0, -24.0), Vec2F(-18.9, -58.0), Vec2F(-0.4, -40.7),  Vec2F(-0.6, -1.4),   Vec2F(-27.6, 6.8),
        Vec2F(-68.6, 8.6),   Vec2F(-116.9, 10.1), Vec2F(27.9, 6.6),    Vec2F(65.5, 8.5),    Vec2F(116.0, 11.1),
        Vec2F(-14.1, 97.1),  Vec2F(-25.2, 165.3), Vec2F(-34.3, 237.5), Vec2F(15.1, 96.6),   Vec2F(24.5, 166.3),
        Vec2F(31.8, 237.7),
    };

    //        // Move to top-left.
    //        for (auto &v: mesh->vertexes) {
    //            v += Vec2F(171, 77);
    //        }

    mesh->polygons.push_back({44, 43, 60, 46, 45});
    mesh->polygons.push_back({47, 60, 46});
    mesh->polygons.push_back({47, 60, 48});
    mesh->polygons.push_back({60, 43, 42});
    mesh->polygons.push_back({60, 41, 42});
    mesh->polygons.push_back({48, 49, 59, 40, 41, 60});
    mesh->polygons.push_back({49, 59, 50});
    mesh->polygons.push_back({50, 59, 51});
    mesh->polygons.push_back({52, 58, 53});
    mesh->polygons.push_back({53, 54, 58});
    mesh->polygons.push_back({59, 58, 52, 51});
    mesh->polygons.push_back({59, 40, 39, 65, 68, 17, 16, 62, 58});
    mesh->polygons.push_back({58, 2, 3});
    mesh->polygons.push_back({58, 4, 3});
    mesh->polygons.push_back({54, 2, 58});
    mesh->polygons.push_back({55, 56, 0, 1, 2, 54});
    mesh->polygons.push_back({58, 4, 5, 62});
    mesh->polygons.push_back({5, 62, 6});
    mesh->polygons.push_back({62, 7, 6});
    mesh->polygons.push_back({62, 16, 15, 63, 8, 7});
    mesh->polygons.push_back({8, 9, 63});
    mesh->polygons.push_back({9, 10, 63});
    mesh->polygons.push_back({15, 14, 63});
    mesh->polygons.push_back({14, 13, 63});
    mesh->polygons.push_back({63, 10, 11, 12, 13});
    mesh->polygons.push_back({68, 17, 18});
    mesh->polygons.push_back({68, 19, 18});
    mesh->polygons.push_back({27, 69, 20, 19, 68, 28});
    mesh->polygons.push_back({65, 28, 68});
    mesh->polygons.push_back({65, 28, 29, 66, 36, 37});
    mesh->polygons.push_back({65, 37, 38});
    mesh->polygons.push_back({65, 39, 38});
    mesh->polygons.push_back({36, 66, 35});
    mesh->polygons.push_back({35, 34, 66});
    mesh->polygons.push_back({66, 29, 30});
    mesh->polygons.push_back({66, 31, 30});
    mesh->polygons.push_back({66, 31, 32, 33, 34});
    mesh->polygons.push_back({27, 69, 26});
    mesh->polygons.push_back({26, 25, 69});
    mesh->polygons.push_back({69, 20, 21});
    mesh->polygons.push_back({22, 21, 69});
    mesh->polygons.push_back({69, 22, 23, 24, 25});

    root_bone->weights = {0,   0, 0,   0.5, 0.5,  0.5,  0.5, 0, 0,   0, 0, 0,   0, 0, 0,    0, 1,   1,
                          0.5, 0, 0,   0,   0,    0,    0,   0, 0,   0, 0, 0,   0, 0, 0,    0, 0,   0,
                          0,   0, 0.5, 1,   0.94, 0,    0,   0, 0,   0, 0, 0,   0, 0, 0.47, 1, 0.5, 0.5,
                          0,   0, 0,   0,   1,    0.47, 0,   0, 0.5, 0, 0, 0.5, 0, 0, 0.5,  0, 0};
    neck_bone->weights = {1, 1, 1, 0.5, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0,   0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0,   0, 0.5, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    head_bone->weights = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    left_shoulder_bone->weights = {0, 0, 0, 0, 0,    0,    0,    0, 0, 0, 0, 0,    0,    0,    0,    0, 0, 0,
                                   0, 0, 0, 0, 0,    0,    0,    0, 0, 0, 0, 0,    0,    0,    0,    0, 0, 0,
                                   0, 0, 0, 0, 0.47, 0.98, 0.49, 0, 0, 0, 0, 0.49, 0.98, 0.98, 0.49, 0, 0, 0,
                                   0, 0, 0, 0, 0,    0.49, 0.49, 0, 0, 0, 0, 0,    0,    0,    0,    0, 0};
    left_arm_bone->weights = {0, 0, 0, 0, 0, 0, 0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0,    0.98, 0.98, 0.98, 0.98, 0.49, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0.49, 0.98, 0,    0,    0,    0,    0, 0, 0, 0, 0};
    left_hand_bone->weights = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    right_shoulder_bone->weights = {0, 0, 0, 0, 0, 0, 0.5, 0.5, 0.5, 0.5, 0, 0, 0, 0, 0.5, 0.5, 0.5, 0,
                                    0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0, 0, 0, 0, 0,   0,   0,   0,
                                    0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0, 0, 0, 0, 0,   0,   0,   0,
                                    0, 0, 0, 0, 0, 0, 0,   0,   0.5, 0.5, 0, 0, 0, 0, 0,   0,   0};
    right_arm_bone->weights = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 1, 1, 1, 1, 0.5, 0,   0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0,   0,   0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0,   0.5, 0, 0, 0, 0, 0, 0, 0};
    right_hand_bone->weights = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    left_hip_bone->weights = {0, 0, 0, 0, 0,   0, 0,   0, 0, 0, 0, 0,   0, 0, 0,   0, 0, 0,   0,   0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0.5, 1, 0.5, 0, 0, 0, 0, 0.5, 1, 1, 0.5, 0, 0, 0,   0,   0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0,   0, 0,   0, 0, 0, 0, 0,   0, 0, 0,   0, 0, 0.5, 0.5, 0, 0, 0, 0};
    left_leg_bone->weights = {0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0.5, 1, 1, 1, 1, 0.5, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0.5, 1, 0, 0, 0};
    left_foot_bone->weights = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    right_hip_bone->weights = {0, 0, 0,   0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 1, 1,   0.5, 0, 0,
                               0, 0, 0.5, 1, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0,   0,   0, 0,
                               0, 0, 0,   0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0.5, 0.5, 0};
    right_leg_bone->weights = {0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 1,  1,
                               1, 1, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0,  0,
                               0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0.5};
    right_foot_bone->weights = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(Platform::getSingleton()->device, &poolInfo, nullptr, &descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }

    auto device = Platform::getSingleton()->device;
    auto &descriptorSetLayout = RenderServer::getSingleton()->skeleton2dMeshDescriptorSetLayout;

    std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor_pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts.data();

    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptor_set) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    allocate_bone_transforms(bones.size());
    update_bone_rest();

    for (int i = 0; i < bone_count; i++) {
        auto relative_transform_to_rest_pose = bones[i]->get_global_transform() * bones[i]->global_rest_inverse;
        set_bone_transform(i, relative_transform_to_rest_pose);
    }

    upload_bone_transforms();
}

Skeleton2d::~Skeleton2d() {
    vkDestroyDescriptorPool(Platform::getSingleton()->device, descriptor_pool, nullptr);
}

template <class C>
void FreeClear(C &cntr) {
    for (auto it = cntr.begin(); it != cntr.end(); it++) {
        delete *it;
    }
    cntr.clear();
}

bool is_equal(float x, float y, float epsilon = 1e-6) {
    if (std::abs(x - y) < epsilon) return true;
    return false;
}

void Skeleton2d::update(double delta) {
    Vec2I viewport_size = get_viewport_size();

    // Prepare MVP data. We use this matrix to convert a full-screen to the NodeGui's rect.
    ModelViewProjection mvp{};

    // The actual application order of these matrices is reverse.
    // 4.
    mvp.model = glm::translate(
        glm::mat4(1.0f), glm::vec3(position.x / viewport_size.x * 2.0f, position.y / viewport_size.y * 2.0f, 0.0f));
    // 3.
    mvp.model = glm::translate(mvp.model, glm::vec3(-1.0, -1.0, 0.0f));
    // 2.
    mvp.model = glm::scale(mvp.model, glm::vec3(scale.x, scale.y, 1.0f));
    // 1.
    mvp.model = glm::scale(mvp.model, glm::vec3(1.0f / viewport_size.x * 2.0f, 1.0f / viewport_size.y * 2.0f, 1.0f));

    pc_skeleton_transform.mvp = mvp.calculate_mvp();

    float time = CoreServer::get_singleton()->get_elapsed();
    bones[3]->rotation = std::sin(time);
    bones[4]->rotation = std::sin(time);

    bones[6]->rotation = std::cos(time);
    bones[7]->rotation = std::cos(time);

    bones[9]->rotation = std::sin(time);
    bones[10]->rotation = std::sin(time);
    bones[11]->rotation = std::sin(time);

    bones[12]->rotation = std::cos(time);
    bones[13]->rotation = std::cos(time);
    bones[14]->rotation = std::cos(time);

    for (int i = 0; i < bone_count; i++) {
        auto global_transform = bones[i]->get_global_transform();
        auto relative_transform_to_rest_pose = global_transform * bones[i]->global_rest_inverse;
        set_bone_transform(i, relative_transform_to_rest_pose);
    }
    upload_bone_transforms();
}

void Skeleton2d::draw(VkCommandBuffer p_command_buffer) {
    // Draw all triangles with a single draw call.
    VkPipeline pipeline = RenderServer::getSingleton()->skeleton2dMeshGraphicsPipeline;
    VkPipelineLayout pipeline_layout = RenderServer::getSingleton()->skeleton2dMeshPipelineLayout;

    // Upload the model matrix to the GPU via push constants.
    vkCmdPushConstants(p_command_buffer,
                       pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(MvpPushConstant),
                       &pc_skeleton_transform);

    VkBuffer vertexBuffers[] = {mesh->gpu_resources->get_vertex_buffer()};
    RenderServer::getSingleton()->draw_skeleton_2d(p_command_buffer,
                                                   pipeline,
                                                   descriptor_set,
                                                   vertexBuffers,
                                                   mesh->gpu_resources->get_index_buffer(),
                                                   mesh->gpu_resources->get_index_count());

    if (root_bone) {
        root_bone->draw();
    }

    Node2d::draw(p_command_buffer);
}

void Skeleton2d::update_bone_rest() {
    uint32_t vertex_count = mesh->vertexes.size();

    // Vertex input.
    // -------------------------------------
    auto points = mesh->vertexes;
    auto uvs = std::vector<Vec2F>(vertex_count);
    // A vertex can be affected by 4 bones at most.
    auto gpu_bones = std::vector<float>(vertex_count * 4, 0);
    auto gpu_weights = std::vector<float>(vertex_count * 4, 0);
    // -------------------------------------

    // Set UVs.
    if (sprite && sprite->get_texture()) {
        // We assume texture hasn't been rotated, scaled, or moved.
        Transform2 tex_transform;

        auto tex_sizei = sprite->get_texture()->get_size();

        // Map UVs to points.
        if (points.size() == uvs.size()) {
            for (int i = 0; i < vertex_count; i++) {
                uvs[i] = tex_transform * (points[i] + Vec2F(171, 77)) / Vec2F(tex_sizei.x, tex_sizei.y);
            }
        } else {
            // Should not happen.
        }
    }

    // Set bones and weights.
    {
        // bone_weights: Vector[Bone count][Vertex count]
        // Traverse each bone.
        for (int i = 0; i < bones.size(); i++) {
            auto &bone = bones[i];
            if (bone == nullptr) {
                continue;
            }

            auto rest_transform = bone->get_global_rest_transform();

            bone->global_rest_inverse = rest_transform.inverse();

            auto &weights = bone->weights;

            if (weights.size() != points.size()) {
                Logger::error("Weights number is different from points number!", "Skeleton2d");
                continue;
            }

            float bone_index = i;

            // Traverse vertexes to calculate weights.
            // This loop picks 4 largest weights.
            for (int j = 0; j < vertex_count; j++) {
                if (weights[j] == 0) {
                    continue;
                }

                // A vertex can be affected by 4 bones at most.
                for (int k = 0; k < 4; k++) {
                    // Bigger than an existing weight, insert new weight!
                    if (weights[j] > gpu_weights[j * 4 + k]) {
                        for (int l = 3; l > k; l--) {
                            gpu_weights[j * 4 + l] = gpu_weights[j * 4 + l - 1];
                            gpu_bones[j * 4 + l] = gpu_bones[j * 4 + l - 1];
                        }
                        gpu_weights[j * 4 + k] = weights[j];
                        gpu_bones[j * 4 + k] = bone_index;
                        break;
                    }
                }
            }
        }

        // Normalize the weights.
        for (int i = 0; i < vertex_count; i++) {
            // Sum up weights.
            float weight_sum = 0;
            for (int j = 0; j < 4; j++) {
                weight_sum += gpu_weights[i * 4 + j];
            }

            // Zero weight, do nothing.
            if (weight_sum == 0) {
                continue;
            }

            // Normalize.
            for (int j = 0; j < 4; j++) {
                gpu_weights[i * 4 + j] /= weight_sum;
            }
        }
    }

    // Final indices.
    std::vector<uint32_t> total_indices;

    // Go through polygons.
    for (auto &polygon : mesh->polygons) {
        uint32_t polygon_vertex_count = polygon.size();

        // Invalid polygon.
        if (polygon_vertex_count < 3) {
            continue;
        }

        // Constrained triangles.
        std::vector<p2t::Triangle *> triangles;
        // Triangle map.
        std::list<p2t::Triangle *> map;
        // Polylines.
        std::vector<std::vector<p2t::Point *>> polylines;
        std::vector<p2t::Point *> polyline;

        // Get vertexes in this polygon.
        std::vector<Vec2F> polygon_vertexes;
        polygon_vertexes.resize(polygon_vertex_count);

        for (int i = 0; i < polygon_vertex_count; i++) {
            // Vertex indices.
            uint32_t idx = polygon[i];
            assert(idx < mesh->vertexes.size());
            polygon_vertexes[i] = mesh->vertexes[idx];

            // Add points to P2T.
            polyline.push_back(new p2t::Point(polygon_vertexes[i].x, polygon_vertexes[i].y));
        }

        polylines.push_back(polyline);

        std::vector<int> new_local_indices;

        // Generate triangles using P2T. We should not generate any new vertexes.
        {
            auto *cdt = new p2t::CDT(polyline);
            cdt->Triangulate();
            triangles = cdt->GetTriangles();

            for (auto tri : triangles) {
                for (int i = 0; i < 3; i++) {
                    auto p = tri->GetPoint(i);

                    bool found_vertex = false;
                    for (int j = 0; j < polygon_vertex_count; j++) {
                        if (is_equal(p->x, polygon_vertexes[j].x) && is_equal(p->y, polygon_vertexes[j].y)) {
                            new_local_indices.push_back(j);
                            found_vertex = true;
                        }
                    }

                    if (!found_vertex) {
                        Logger::error(
                            "Couldn't find the same vertex during triangulation. A new vertex has been generated, "
                            "which is not allowed!",
                            "Skeleton2D");
                    }
                }
            }

            delete cdt;

            // Free points.
            for (auto poly : polylines) {
                FreeClear(poly);
            }
        }

        uint32_t new_index_count = new_local_indices.size();

        // Add triangle indices to total indices.
        uint32_t index_count = total_indices.size();
        total_indices.resize(index_count + new_index_count);

        // Add new indices.
        for (int i = 0; i < new_index_count; i++) {
            // Global index.
            uint32_t global_idx = polygon[new_local_indices[i]];
            total_indices[index_count + i] = global_idx;
        }
    }

    // Upload to vertex buffer and index buffer.
    auto vertex_data = std::vector<SkeletonVertex>(vertex_count);

    for (int i = 0; i < vertex_count; i++) {
        vertex_data[i].pos = {points[i].x, points[i].y, 0};
        vertex_data[i].uv = {uvs[i].x, uvs[i].y};
        vertex_data[i].bone_indices = {
            gpu_bones[i * 4], gpu_bones[i * 4 + 1], gpu_bones[i * 4 + 2], gpu_bones[i * 4 + 3]};
        vertex_data[i].bone_weights = {
            gpu_weights[i * 4], gpu_weights[i * 4 + 1], gpu_weights[i * 4 + 2], gpu_weights[i * 4 + 3]};
    }

    mesh->gpu_resources = std::make_shared<VertexGpuResources<SkeletonVertex>>(vertex_data, total_indices);
}

void Skeleton2d::allocate_bone_transforms(uint32_t new_bone_count) {
    if (bone_count == new_bone_count) {
        return;
    }

    bone_count = new_bone_count;

    int width = 256;
    int height = (int)bone_count / width;
    if (bone_count % width) height++;

    uint32_t tex_width = width;
    uint32_t tex_height = height * (true ? 2 : 3); // 2D/3D.

    bone_transforms.resize(tex_width * tex_height * 4);
    bone_transforms_texture = ImageTexture::from_empty(tex_width, tex_height, VK_FORMAT_R32G32B32A32_SFLOAT);

    // Need a sampler of nearest.
    bone_transforms_texture->set_filter(VK_FILTER_NEAREST);

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = bone_transforms_texture->imageView;
    imageInfo.sampler = bone_transforms_texture->sampler;

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptor_set;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pImageInfo = &imageInfo;

    VkDescriptorImageInfo imageInfo2{};
    imageInfo2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo2.imageView = sprite->get_texture()->imageView;
    imageInfo2.sampler = sprite->get_texture()->sampler;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptor_set;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo2;

    // Update the contents of a descriptor set object.
    vkUpdateDescriptorSets(Platform::getSingleton()->device,
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(),
                           0,
                           nullptr);
}

void Skeleton2d::upload_bone_transforms() {
    if (bone_count > 0) {
        int width = 256;
        int height = (int)bone_count / width;
        if (bone_count % width) height++;

        uint32_t tex_width = width;
        uint32_t tex_height = height * 2;

        // Upload the bone transform data to the texture.
        // In bytes. 16 bytes per R32G32B32A32F pixel.
        VkDeviceSize imageSize = tex_width * tex_height * 16;

        // Temporary buffer and device memory.
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        RenderServer::getSingleton()->createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            staging_buffer,
            staging_buffer_memory);

        // Copy the pixel values that we got from the image loading library to the buffer.
        RenderServer::getSingleton()->copyDataToMemory(bone_transforms.data(), staging_buffer_memory, imageSize);

        // Transition the texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
        RenderServer::getSingleton()->transitionImageLayout(bone_transforms_texture->image,
                                                            VK_FORMAT_R32G32B32A32_SFLOAT,
                                                            VK_IMAGE_LAYOUT_UNDEFINED,
                                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Execute the buffer to image copy operation.
        RenderServer::getSingleton()->copyBufferToImage(staging_buffer,
                                                        bone_transforms_texture->image,
                                                        0,
                                                        0,
                                                        static_cast<uint32_t>(tex_width),
                                                        static_cast<uint32_t>(tex_height));

        // To be able to start sampling from the texture image in the shader, we need one last transition to prepare it
        // for shader access.
        RenderServer::getSingleton()->transitionImageLayout(bone_transforms_texture->image,
                                                            VK_FORMAT_R32G32B32A32_SFLOAT,
                                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Clean up staging objects.
        vkDestroyBuffer(Platform::getSingleton()->device, staging_buffer, nullptr);
        vkFreeMemory(Platform::getSingleton()->device, staging_buffer_memory, nullptr);
    }
}

void Skeleton2d::set_bone_transform(uint32_t bone_index, const Transform2 &p_transform) {
    if (bone_index >= bone_count) {
        Logger::error("Invalid bone index!", "Skeleton 2D");
        return;
    }

    // Data offset.
    uint32_t row = (bone_index / 256) * 2;
    uint32_t col = bone_index % 256;
    uint32_t offset = (row * 256 + col) * 4;

    // First row.
    bone_transforms[offset + 0] = p_transform.m11();
    bone_transforms[offset + 1] = p_transform.m12();
    bone_transforms[offset + 2] = 0;
    bone_transforms[offset + 3] = p_transform.get_position().x;

    // Second row.
    offset += 256 * 4;
    bone_transforms[offset + 0] = p_transform.m21();
    bone_transforms[offset + 1] = p_transform.m22();
    bone_transforms[offset + 2] = 0;
    bone_transforms[offset + 3] = p_transform.get_position().y;
}
} // namespace Flint
