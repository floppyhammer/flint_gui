#ifndef FLINT_SUB_VIEWPORT_CONTAINER_H
#define FLINT_SUB_VIEWPORT_CONTAINER_H

#include "container.h"
#include "../sub_viewport.h"

namespace Flint {
    class SubViewportContainer : public Container {
    public:
        SubViewportContainer();

        void set_viewport(std::shared_ptr<SubViewport> p_viewport);

        void _update(double delta) override;

        void _draw(VkCommandBuffer p_command_buffer) override;

    protected:
        std::shared_ptr<SubViewport> viewport;

    private:
        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        std::shared_ptr<Mesh2d> mesh;
    };
}

#endif //FLINT_SUB_VIEWPORT_CONTAINER_H
