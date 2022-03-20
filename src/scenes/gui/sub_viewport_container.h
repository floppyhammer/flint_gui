#ifndef FLINT_SUB_VIEWPORT_CONTAINER_H
#define FLINT_SUB_VIEWPORT_CONTAINER_H

#include "control.h"
#include "../sub_viewport.h"

namespace Flint {
    class SubViewportContainer : public Control {
    public:
        SubViewportContainer();

        void set_viewport(std::shared_ptr<SubViewport> p_viewport);

    protected:
        std::shared_ptr<SubViewport> viewport;

    protected:
        void update(double delta) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        void self_draw(VkCommandBuffer p_command_buffer);

        void create_descriptor_pool();

        void create_descriptor_sets();

        void update_descriptor_sets();
    };
}

#endif //FLINT_SUB_VIEWPORT_CONTAINER_H
