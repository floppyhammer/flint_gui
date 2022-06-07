#ifndef FLINT_SKELETON_2D_H
#define FLINT_SKELETON_2D_H

#include "node_2d.h"
#include "sprite_2d.h"
#include "../../../resources/mesh.h"
#include "../../../render/mvp_buffer.h"
#include "../../../servers/vector_server.h"

namespace Flint {
    class Skeleton2d;

    struct Bone2d {
        // For clarity.
        std::string name;
        // Starting point, which is relative to parent. This doesn't change once the rest pose is set.
        Vec2F position;
        // Rotation in radians, which is relative to parent.
        float rotation;
        // Length of the bone. Only for terminal bones.
        float length = 48;
        // Parent transform cache.
        Transform2 parent_transform;
        // Bone flags, 8 bits should be sufficient for now.
        uint8_t flags;
        // Pointers to children.
        std::vector<std::shared_ptr<Bone2d>> children;
        // Pointer to parent bone.
        Bone2d *parent;
        // Pointer to parent skeleton, only for root bone.
        Skeleton2d *skeleton;

        Transform2 global_rest_inverse;

        // Vector[Vertex count]
        std::vector<float> weights;

        void add_child(const std::shared_ptr<Bone2d> &child);

        Skeleton2d *get_skeleton();

        void draw();

        Transform2 get_transform();

        Transform2 get_global_transform();

        Transform2 get_rest_transform();

        Transform2 get_global_rest_transform();
    };

    struct Skeleton2dMesh {
        // Including internal vertexes, which are placed at the end of the vector.
        std::vector<Vec2F> vertexes;
        uint32_t internal_vertices;

        // Vector[polygon count][vertex count in a polygon]
        std::vector<std::vector<uint32_t>> polygons;

        // Vector[bone count][vertex count]
        std::vector<std::vector<float>> bone_weights;

        std::shared_ptr<SurfaceGpuResources<SkeletonVertex>> gpu_resources;
    };

    class Skeleton2d : public Node2d {
        friend Bone2d;
    public:
        Skeleton2d();

    private:
        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        std::shared_ptr<Bone2d> root_bone;

        std::shared_ptr<Sprite2d> sprite;

        std::vector<Bone2d *> bones;

        std::shared_ptr<Skeleton2dMesh> mesh;

        Surface2dPushConstant pc_skeleton_transform;

        std::vector<float> bone_transforms;
        std::shared_ptr<ImageTexture> bone_transforms_texture;

        uint32_t bone_count;

        VkDescriptorPool descriptor_pool;
        VkDescriptorSet descriptor_set;

        /// When bone vertexes, weights, or polygons changes, update the vertex buffer.
        /// Bone transforms are updated through update_bone_transforms().
        void update_bone_rest();

        void allocate_bone_transforms(uint32_t new_bone_count);

        /// Bone's global transform relative to the rest pose, i.e. how far it transforms from the rest pose.
        void set_bone_transform(uint32_t bone_index, const Transform2 &transform);

        void upload_bone_transforms();
    };
}

#endif //FLINT_SKELETON_2D_H
