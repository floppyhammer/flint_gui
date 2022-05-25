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

    class Skeleton2d : public Node2d {
    public:
        Skeleton2d();

        void set_texture(std::shared_ptr<ImageTexture> p_texture);

        [[nodiscard]] std::shared_ptr<ImageTexture> get_texture() const;

        void set_mesh(const std::shared_ptr<Mesh2d>& p_mesh);

        void set_material(const std::shared_ptr<Material2d>& p_material);

    private:
        std::shared_ptr<Bone2d> base_bone;

        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        void update_mvp();

        Surface2dPushConstant push_constant;
    };
}

#endif //FLINT_SKELETON_2D_H
