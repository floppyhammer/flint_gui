#include "scroll_container.h"

#include "pathfinder.h"

using Pathfinder::clamp;

namespace Flint {

ScrollContainer::ScrollContainer() {
    type = NodeType::ScrollContainer;

    theme_scroll_bar.corner_radius = 0;

    theme_scroll_grabber.bg_color = ColorU(163, 163, 163, 255);
    theme_scroll_grabber.corner_radius = 0;

    test_content = std::make_shared<Tree>();
    test_content->set_size({400, 600});
}

void ScrollContainer::adjust_layout() {
    if (children.empty()) return;
}

void ScrollContainer::input(std::vector<InputEvent> &input_queue) {
    if (mouse_filter != MouseFilter::Stop) {
        return;
    }

    auto global_position = get_global_position();

    auto active_rect = Rect<float>(global_position, global_position + size);

    // Handle mouse input propagation.
    for (auto &event : input_queue) {
        bool consume_flag = false;

        switch (event.type) {
            case InputEventType::MouseScroll: {
                float delta = event.args.mouse_scroll.y_delta;

                if (active_rect.contains_point(InputServer::get_singleton()->cursor_position)) {
                    if (!event.is_consumed()) {
                        vscroll -= delta * 10;

                        auto grabber_size = Vec2F(16, size.y / test_content->get_size().y * size.y);
                        vscroll = clamp(vscroll, 0, int32_t(size.y - grabber_size.y));
                    }

                    consume_flag = true;
                } else {
                }
            } break;
            default:
                break;
        }

        if (consume_flag) {
            event.consume();
        }
    }

    Control::input(input_queue);
}

void ScrollContainer::update(double dt) {
    auto self_size = size.y;
    auto scroller_size = 800;

    Control::update(dt);
}

Vec2<float> ScrollContainer::calculate_minimum_size() const {
    Vec2<float> min_size;

    return min_size.max(minimum_size);
}

void ScrollContainer::draw(VkCommandBuffer p_command_buffer) {
    auto vector_server = VectorServer::get_singleton();

    auto global_pos = get_global_position();
    auto size = get_size();

    auto scroll_bar_pos = Vec2F(global_pos.x + size.x - 16, global_pos.y);
    auto scroll_bar_size = Vec2F(16, size.y);

    theme_scroll_bar.add_to_canvas(scroll_bar_pos, scroll_bar_size, vector_server->canvas);

    auto grabber_pos = Vec2F(global_pos.x + size.x - 16, global_pos.y + vscroll);
    auto grabber_size = Vec2F(16, size.y / test_content->get_size().y * size.y);

    theme_scroll_grabber.add_to_canvas(grabber_pos, grabber_size, vector_server->canvas);

    test_content->set_position(global_pos - Vec2F(0, (float)vscroll));

    vector_server->set_clipping_box({position, position + size});
    test_content->draw(p_command_buffer);
    vector_server->unset_clipping_box();

    Control::draw(p_command_buffer);
}

void ScrollContainer::set_hscroll(int32_t value) {
    if (children.empty()) return;

    Vec2F max_child_min_size = get_max_child_min_size();

    hscroll = std::min(0.0f, max_child_min_size.x - size.x);
}

int32_t ScrollContainer::get_hscroll() {
    return hscroll;
}

void ScrollContainer::set_vscroll(int32_t value) {
    if (children.empty()) return;

    Vec2F max_child_min_size = get_max_child_min_size();

    vscroll = std::min(0.0f, max_child_min_size.y - size.y);
}

int32_t ScrollContainer::get_vscroll() {
    return vscroll;
}

} // namespace Flint
