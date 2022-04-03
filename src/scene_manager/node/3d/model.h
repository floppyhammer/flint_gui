#ifndef FLINT_MODEL_H
#define FLINT_MODEL_H

#include "node_3d.h"
#include "../../../rendering/mesh.h"
#include "../../../rendering/texture.h"

namespace Flint {
    class MeshInstance3D : public Node3D {
    public:
        MeshInstance3D();
        ~MeshInstance3D();

    protected:
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<Material> materials;

        void _draw(VkCommandBuffer p_command_buffer) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        void _update(double delta) override;

        void loadFile(const std::string &filename);
    };
}

#endif //FLINT_MODEL_H
