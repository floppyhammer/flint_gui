#ifndef FLINT_SKYBOX_H
#define FLINT_SKYBOX_H

#include "../../resources/cube_texture.h"
#include "../../resources/mesh.h"
#include "node3d.h"

namespace Flint {

// TODO: skybox should not be a node.
class Skybox : public Node3d {
public:
    Skybox();

protected:
    void update(double dt) override;

    void draw(VkCommandBuffer cmd_buffer) override;

    std::shared_ptr<MaterialSkybox> material;

    std::shared_ptr<VertexGpuResources<SkyboxVertex>> surface_gpu_resources;
};

} // namespace Flint

#endif // FLINT_SKYBOX_H
