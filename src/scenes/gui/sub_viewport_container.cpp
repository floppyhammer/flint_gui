#include "sub_viewport_container.h"

#include "../sub_viewport.h"
#include "../../rendering/swap_chain.h"

#include <utility>
#include <array>

namespace Flint {
    SubViewportContainer::SubViewportContainer() {
        type = NodeType::SubViewportContainer;
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

        // Don't call Control::draw(), so we can break the recursive calling
        // to call the sub-viewport draw function below specifically.

        // Start sub-viewport render pass.
        if (viewport != nullptr) {
            viewport->draw();
        }

        // Bind the main viewport render pass again.
        // Begin render pass.
        // ----------------------------------------------
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = SwapChain::getSingleton().renderPass;
        renderPassInfo.framebuffer = SwapChain::getSingleton().swapChainFramebuffers[SwapChain::getSingleton().currentImage]; // Set target framebuffer.
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = SwapChain::getSingleton().swapChainExtent;

        // Clear color.
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer,
                             &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
        // ----------------------------------------------

        // Now draw the sub-viewport image.
        // TODO: Finish TextureRect node first.

        Logger::verbose("DRAW", "SubViewportContainer");
    }
}
