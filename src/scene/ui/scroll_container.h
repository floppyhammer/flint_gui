#ifndef FLINT_SCROLL_CONTAINER_H
#define FLINT_SCROLL_CONTAINER_H

#include "container.h"
#include "tree.h"

namespace Flint {

class ScrollContainer : public Container {
public:
    ScrollContainer();

    void input(InputEvent &event) override;

    void update(double dt) override;

    void draw() override;

    void adjust_layout() override;

    Vec2F calc_minimum_size() const override;

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

    Pathfinder::RenderTarget sub_render_target;

    StyleBox theme_scroll_bar;
    StyleBox theme_scroll_grabber;

private:
    void propagate_draw(VkCommandBuffer p_command_buffer) override;

    void propagate_input(InputEvent &event) override;
};

} // namespace Flint

#endif // FLINT_SCROLL_CONTAINER_H
