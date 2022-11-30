#ifndef FLINT_SUB_VIEWPORT_CONTAINER_H
#define FLINT_SUB_VIEWPORT_CONTAINER_H

#include "container.h"
#include "scene/sub_viewport.h"

namespace Flint {
class SubViewportContainer : public Container {
public:
    SubViewportContainer();

    void set_viewport(std::shared_ptr<SubViewport> p_viewport);

    void propagate_draw(VkCommandBuffer p_command_buffer) override;

protected:
    std::shared_ptr<SubViewport> sub_viewport;

private:
    void update(double dt) override;

    void draw(VkCommandBuffer p_command_buffer) override;

    void update_mvp();

    std::shared_ptr<Mesh2d> mesh;

    MvpPushConstant push_constant;
};
} // namespace Flint

#endif // FLINT_SUB_VIEWPORT_CONTAINER_H
