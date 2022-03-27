#ifndef FLINT_MODEL_H
#define FLINT_MODEL_H

#include "node_3d.h"
#include "../../rendering/mesh.h"
#include "../../rendering/texture.h"

namespace Flint {
    class MeshInstance3D : public Node3D {
    public:
        MeshInstance3D();
        ~MeshInstance3D();

    protected:
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<Material> materials;

        void draw(VkCommandBuffer p_command_buffer) override;

        void self_draw(VkCommandBuffer p_command_buffer) override;

        void update(double delta) override;

        void loadFile(const std::string &filename, const std::string &mat_base);
    };
}

#endif //FLINT_MODEL_H
