#ifndef FLINT_WINDOW_PROXY_H
#define FLINT_WINDOW_PROXY_H

#include "../render/swap_chain.h"
#include "node.h"

using Pathfinder::ColorF;
using Pathfinder::Vec2I;

namespace Flint {

class WindowProxy : public Node {
    friend class SceneTree;

public:
    WindowProxy(Vec2I _size, bool _dummy);

    void propagate_input(InputEvent &event) override;

    void propagate_update(double dt) override;

    void propagate_draw(VkRenderPass render_pass, VkCommandBuffer cmd_buffer) override;

    Vec2I get_size() const;

    std::shared_ptr<Window> get_real() {
        return window;
    }

    std::shared_ptr<SwapChain> swapchain;

private:
    /// Window size.
    Vec2I size;

    // Dummy window for headless use.
    bool dummy = false;

    std::shared_ptr<Window> window;

    void record_commands(std::vector<VkCommandBuffer> &command_buffers, uint32_t image_index) const;
};

} // namespace Flint

#endif // FLINT_WINDOW_PROXY_H
