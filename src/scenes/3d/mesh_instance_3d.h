#ifndef FLINT_MESH_INSTANCE_3D_H
#define FLINT_MESH_INSTANCE_3D_H

#include "node_3d.h"
#include "../../rendering/mesh.h"
#include "../../rendering/texture.h"

namespace Flint {
    class MeshInstance3D : public Node3D {
    public:
        void set_mesh(std::shared_ptr<Mesh> p_mesh);

        std::shared_ptr<Mesh> get_mesh() const;

        void self_draw() override;

        void cleanup() override;

    protected:
        std::shared_ptr<Texture> texture;

        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        void createGraphicsPipeline();
    };
}

#endif //FLINT_MESH_INSTANCE_3D_H
