#ifndef FLINT_SKELETON_2D_H
#define FLINT_SKELETON_2D_H

#include "node_2d.h"
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
        // Zero when the bone points to the same direction of the parent-to-self arrow.
        float rotation;
        // Length of the bone. Only for terminal bones.
        float length = 48;
        // Parent transform cache.
        Transform2 parent_transform;
        // Bone flags, 8 bits should be sufficient for now.
        uint8_t flags;
        // Number of children.
        uint8_t child_count;
        // Pointers to children.
        std::vector<std::shared_ptr<Bone2d>> children;
        // Pointer to parent.
        Bone2d *parent;
        Skeleton2d *skeleton;

        void add_child(std::shared_ptr<Bone2d> child);

        void draw();

        Transform2 get_transform();

        Transform2 get_global_transform();
    };

    struct BoneVertex {
        Vertex v; // Info on this vertex: position, color etc.
        std::vector<float> weights;	// Weight for each bone connected.
        std::vector<Bone2d *> bones; // Pointer to connected bones.
    };

    struct Skeleton2dMesh {
        std::vector<BoneVertex> vertexes;
        // Triangles made up of vertexes.
        std::vector<uint32_t> triangles;
    };

    class Skeleton2d : public Node2d {
    public:
        Skeleton2d();

    private:
        std::shared_ptr<Bone2d> base_bone;

        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;
    };
}

#endif //FLINT_SKELETON_2D_H
