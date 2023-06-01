#ifndef FLINT_MODEL_H
#define FLINT_MODEL_H

#include "../../resources/image_texture.h"
#include "../../resources/mesh.h"
#include "node3d.h"

namespace Flint {

class Model : public Node3d {
public:
    Model();

    void set_mesh(std::shared_ptr<Mesh3d> new_mesh);

protected:
    std::shared_ptr<Mesh3d> mesh;

    void update(double dt) override;

    void draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;
};

} // namespace Flint

#endif // FLINT_MODEL_H
