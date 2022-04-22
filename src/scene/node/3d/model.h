#ifndef FLINT_MODEL_H
#define FLINT_MODEL_H

#include "node_3d.h"
#include "../../../resources/mesh.h"
#include "../../../resources/texture.h"

namespace Flint {
    class Model : public Node3D {
    public:
        Model();

        void _update(double delta) override;

        void _draw(VkCommandBuffer p_command_buffer) override;

    protected:
        std::vector<std::shared_ptr<Mesh3d>> meshes;
        std::vector<std::shared_ptr<Mesh3dDescSet>> desc_sets;
        std::vector<std::shared_ptr<Material3d>> materials;

        void draw(VkCommandBuffer p_command_buffer) override;

        void load_file(const std::string &filename);
    };
}

#endif //FLINT_MODEL_H
