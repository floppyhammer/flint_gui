#pragma once

#include "../common/geometry.h"
#include "node.h"

namespace revector {

/// A sub-window besides the primary window.
class SubWindow : public Node {
    friend class SceneTree;

public:
    explicit SubWindow(Vec2I size);

    void update(double dt) override;

    void pre_draw_children() override;

    void post_draw_children() override;

    Vec2I get_size() const;

    void set_visibility(bool visible) override;

    std::shared_ptr<Pathfinder::Window> get_raw_window() const;

    // std::shared_ptr<Pathfinder::SwapChain> get_swap_chain() const {
    //     return swap_chain_;
    // }

    std::shared_ptr<Pathfinder::Texture> get_vector_target() const {
        return vector_target_;
    }

    void set_vector_target(std::shared_ptr<Pathfinder::Texture> texture) {
        vector_target_ = texture;
    }

protected:
    Vec2I size_;

    uint8_t window_index_;
    // std::shared_ptr<Pathfinder::SwapChain> swap_chain_;
    std::shared_ptr<Pathfinder::Texture> vector_target_;

    void record_commands() const;

private:
    struct {
        std::shared_ptr<Pathfinder::Scene> previous_scene;
    } temp_draw_data;

    float scale_factor = 1.0;
};

} // namespace revector
