#include "sub_viewport_container.h"

#include "../sub_viewport.h"
#include "../../rendering/swap_chain.h"

#include <utility>

namespace Flint {
    std::shared_ptr<SubViewport> SubViewportContainer::get_viewport() {
        return {viewport};
    }

    void SubViewportContainer::set_viewport(std::shared_ptr<SubViewport> p_viewport) {
        viewport = std::move(p_viewport);
    }

    void SubViewportContainer::update(double delta) {
        // Branch to root.
        Control::update(delta);
    }

    void SubViewportContainer::draw() {
        auto commandBuffer = SwapChain::getSingleton().commandBuffers[SwapChain::getSingleton().currentImage];

        // Pause command recording for the main viewport, and start recording commands for the sub viewport.
        vkCmdEndRenderPass(commandBuffer);

        // End recording.
        if (viewport != nullptr) {
            viewport->draw();
        }

        // Bind the main viewport render pass again.


        // Now draw the viewport image.

        // Don't call parent class draw(), so we can break the recursive calling.
        // Control::draw();
    }
}
