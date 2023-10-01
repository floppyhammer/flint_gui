#ifndef FLINT_MODEL_H
#define FLINT_MODEL_H

#include "../../resources/image_texture.h"
#include "../../resources/surface.h"
#include "node3d.h"

namespace Flint {

class Model : public Node3d {
public:
    Model();

    void set_surfaces(const std::vector<std::shared_ptr<Surface3d>> &new_surfaces);

protected:
    void update(double dt) override;

    void draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    std::vector<std::shared_ptr<Surface3d>> surfaces;
};

} // namespace Flint

#endif // FLINT_MODEL_H
