#ifndef FLINT_MESH_INSTANCE_3D_H
#define FLINT_MESH_INSTANCE_3D_H

#include "node_3d.h"
#include "../../rendering/mesh.h"
#include "../../rendering/texture.h"

namespace Flint {
    class MeshInstance3D : public Node3D {
    public:
        MeshInstance3D();

        void set_mesh(std::shared_ptr<Mesh> p_mesh);
        [[nodiscard]] std::shared_ptr<Mesh> get_mesh() const;

        void set_texture(std::shared_ptr<Texture> p_texture);

        void cleanup() override;

    protected:
        std::shared_ptr<Texture> texture;

        void self_draw() override;
        void self_update(double delta) override;

        void createDescriptorSets();

        // Should be recalled once texture is changed.
        void updateDescriptorSets();
    };
}

#endif //FLINT_MESH_INSTANCE_3D_H
