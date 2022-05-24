#include "sub_viewport_system.h"

#include "../components/components.h"
#include "../coordinator.h"
#include "../../../render/swap_chain.h"

namespace Flint {
    void SubViewportInputSystem::update(const std::weak_ptr<Coordinator>& p_coordinator) {
        auto command_buffer = RenderServer::getSingleton()->beginSingleTimeCommands();

        auto coordinator = p_coordinator.lock();

        for (auto const &entity: entities) {
            auto &viewport_component = coordinator->get_component<ViewportInputComponent>(entity);

        }

//        // End render pass.
//        vkCmdEndRenderPass(p_command_buffer);
//
//        RenderServer::getSingleton()->endSingleTimeCommands(sub_viewport_command_buffer);
    }

    void SubViewportOutSystem::update(const std::weak_ptr<Coordinator> &p_coordinator) {

    }
}
