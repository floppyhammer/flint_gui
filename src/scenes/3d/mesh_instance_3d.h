#ifndef FLINT_MESH_INSTANCE_3D_H
#define FLINT_MESH_INSTANCE_3D_H

#include "node_3d.h"
#include "../../rendering/mesh.h"
#include "../../rendering/texture.h"

namespace Flint {
    class MeshInstance3D : public Node3D {
    public:
        MeshInstance3D();
        ~MeshInstance3D();

        void set_mesh(std::shared_ptr<Mesh> p_mesh);
        [[nodiscard]] std::shared_ptr<Mesh> get_mesh() const;

        void set_texture(std::shared_ptr<Texture> p_texture);

    protected:
        std::shared_ptr<Texture> texture;

        void draw() override;
        void update(double delta) override;

        /**
         * A descriptor pool is used to allocate descriptor sets of some layout for use in a shader.
         * @dependency None.
         */
        void createDescriptorPool();

        /**
         * Allocate descriptor sets in the pool.
         * @dependency Descriptor pool, descriptor set layout, and actual resources (uniform buffers, images, image views).
         */
        void createDescriptorSets();

        // Should be recalled once texture is changed.
        void updateDescriptorSets();
    };
}

#endif //FLINT_MESH_INSTANCE_3D_H
