#ifndef FLINT_NODE_SUB_WINDOW_PROXY_H
#define FLINT_NODE_SUB_WINDOW_PROXY_H

#include "../common/geometry.h"
#include "node.h"

namespace Flint {

/// A sub-window besides the primary window.
class SubWindow : public Node {
    friend class SceneTree;

public:
    explicit SubWindow(Vec2I size);

    void propagate_input(InputEvent &event) override;

    void update(double dt) override;

    void begin_draw();

    void end_draw();

    Vec2I get_size() const;

    void set_visibility(bool visible) override;

    std::shared_ptr<Pathfinder::Window> get_raw_window() const {
        return window_;
    }

    std::shared_ptr<Pathfinder::SwapChain> get_swap_chain() const {
        return swap_chain_;
    }

    std::shared_ptr<Pathfinder::Texture> get_vector_target() const {
        return vector_target_;
    }

    void set_vector_target(std::shared_ptr<Pathfinder::Texture> texture) {
        vector_target_ = texture;
    }

protected:
    Vec2I size_;

    std::shared_ptr<Pathfinder::Window> window_;
    std::shared_ptr<Pathfinder::SwapChain> swap_chain_;
    std::shared_ptr<Pathfinder::Texture> vector_target_;

    void record_commands() const;
};

} // namespace Flint

#endif // FLINT_NODE_SUB_WINDOW_PROXY_H
