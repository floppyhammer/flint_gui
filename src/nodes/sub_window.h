#ifndef FLINT_WINDOW_PROXY_H
#define FLINT_WINDOW_PROXY_H

#include "../common/geometry.h"
#include "node.h"

namespace Flint {

class SubWindow : public Node {
    friend class SceneTree;

public:
    explicit SubWindow(Vec2I _size);

    void propagate_input(InputEvent &event) override;

    void propagate_update(double dt) override;

    void propagate_draw() override;

    Vec2I get_size() const;

    void set_visibility(bool _visible) override;

private:
    Vec2I size_;

    std::shared_ptr<Pathfinder::Window> window_;
    std::shared_ptr<Pathfinder::SwapChain> swap_chain_;
    std::shared_ptr<Pathfinder::Texture> vector_target_;

    void record_commands() const;
};

} // namespace Flint

#endif // FLINT_WINDOW_PROXY_H
