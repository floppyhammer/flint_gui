#include "sub_viewport_system.h"

#include "../components/components.h"
#include "../coordinator.h"
#include "../../../rendering/swap_chain.h"

namespace Flint {
    void SubViewportInputSystem::update() {
        auto command_buffer = RS::getSingleton().beginSingleTimeCommands();

        auto coordinator = Coordinator::get_singleton();

        for (auto const &entity: entities) {
            auto &viewport_component = coordinator.get_component<ViewportInputComponent>(entity);

        }

//        // End render pass.
//        vkCmdEndRenderPass(p_command_buffer);
//
//        RS::getSingleton().endSingleTimeCommands(sub_viewport_command_buffer);
    }
}
