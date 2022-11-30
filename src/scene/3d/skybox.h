#ifndef FLINT_SKYBOX_H
#define FLINT_SKYBOX_H

#include "glm/glm.hpp"
#include "node_3d.h"
#include "resources/cubemap_texture.h"
#include "resources/mesh.h"

namespace Flint {
class Skybox : public Node3D {
public:
    Skybox();

protected:
    void draw(VkCommandBuffer p_command_buffer) override;

    std::shared_ptr<MaterialSkybox> material;

    std::shared_ptr<VertexGpuResources<SkyboxVertex>> surface_gpu_resources;
};
} // namespace Flint

#endif // FLINT_SKYBOX_H
