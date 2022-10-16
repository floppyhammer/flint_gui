#ifndef FLINT_SCROLL_CONTAINER_H
#define FLINT_SCROLL_CONTAINER_H

#include "container.h"
#include "tree.h"

namespace Flint {

class ScrollContainer : public Container {
public:
    ScrollContainer();

    void input(std::vector<InputEvent> &input_queue) override;

    void update(double dt) override;

    void draw(VkCommandBuffer p_command_buffer) override;

    void adjust_layout() override;

    Vec2<float> calculate_minimum_size() const override;

    void set_hscroll(int32_t value);

    int32_t get_hscroll();

    void set_vscroll(int32_t value);

    int32_t get_vscroll();

protected:
    bool horizontal_mode = true;
    bool vertical_mode = true;

    /// Scroll value can be negative to achieve blank scrolling space.
    int32_t hscroll = 0;
    int32_t vscroll = 0;

    /// Scroll container should use a separate Pathfinder::SceneBuilder
    /// and use the ViewBox property to achieve element clipping.
    std::shared_ptr<Pathfinder::SceneBuilder> scene_builder;

    StyleBox theme_scroll_bar;
    StyleBox theme_scroll_grabber;

    std::shared_ptr<Tree> test_content;
};

} // namespace Flint

#endif // FLINT_SCROLL_CONTAINER_H
