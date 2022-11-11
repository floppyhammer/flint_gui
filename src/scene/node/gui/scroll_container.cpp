#include "scroll_container.h"

#include "pathfinder.h"

using Pathfinder::clamp;

namespace Flint {

ScrollContainer::ScrollContainer() {
    type = NodeType::ScrollContainer;

    theme_scroll_bar.corner_radius = 0;

    theme_scroll_grabber.bg_color = ColorU(163, 163, 163, 255);
    theme_scroll_grabber.corner_radius = 0;
}

void ScrollContainer::adjust_layout() {
    if (children.empty()) {
        return;
    }
}

void ScrollContainer::input(std::vector<InputEvent> &input_queue) {
    Control::input(input_queue);

    if (children.empty() || !children.front()->is_gui_node()) {
        return;
    }

    // Scroll container can only have one effective control child.
    auto content = (Control *)children.front().get();

    auto global_position = get_global_position();

    auto active_rect = RectF(global_position, global_position + size);

    // Handle mouse input propagation.
    for (auto &event : input_queue) {
        bool consume_flag = false;

        switch (event.type) {
            case InputEventType::MouseScroll: {
                float delta = event.args.mouse_scroll.y_delta;

                if (active_rect.contains_point(InputServer::get_singleton()->cursor_position)) {
                    if (!event.is_consumed()) {
                        vscroll -= delta * 10;

                        auto grabber_size = Vec2F(16, size.y / content->get_size().y * size.y);
                        vscroll = clamp(vscroll, 0, int32_t(size.y - grabber_size.y));
                    }

                    // Will stop input propagation.
                    if (mouse_filter == MouseFilter::Stop) {
                        consume_flag = true;
                    }
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
}

void ScrollContainer::update(double dt) {
    if (children.empty() || !children.front()->is_gui_node()) {
        return;
    }

    // Scroll container can only have one effective control child.
    auto content = (Control *)children.front().get();

    content->set_position({0, (float)-vscroll});

    Control::update(dt);
}

Vec2F ScrollContainer::calculate_minimum_size() const {
    Vec2F min_size;

    return min_size.max(minimum_size);
}

void ScrollContainer::draw(VkCommandBuffer p_command_buffer) {
    if (children.empty() || !children.front()->is_gui_node()) {
        return;
    }

    // Scroll container can only have one effective control child.
    auto content = (Control *)children.front().get();

    auto vector_server = VectorServer::get_singleton();

    auto global_pos = get_global_position();
    auto size = get_size();

    auto scroll_bar_pos = Vec2F(size.x - 16, 0) + global_pos;
    auto scroll_bar_size = Vec2F(16, size.y);

    vector_server->draw_style_box(theme_scroll_bar, scroll_bar_pos, scroll_bar_size);

    auto grabber_pos = Vec2F(size.x - 16, vscroll) + global_pos;
    auto grabber_size = Vec2F(16, size.y / content->get_size().y * size.y);

    vector_server->draw_style_box(theme_scroll_grabber, grabber_pos, grabber_size);

    Control::draw(p_command_buffer);
}

void ScrollContainer::set_hscroll(int32_t value) {
    if (children.empty()) {
        return;
    }

    Vec2F max_child_min_size = get_max_child_min_size();

    hscroll = std::min(0.0f, max_child_min_size.x - size.x);
}

int32_t ScrollContainer::get_hscroll() {
    return hscroll;
}

void ScrollContainer::set_vscroll(int32_t value) {
    if (children.empty()) {
        return;
    }

    Vec2F max_child_min_size = get_max_child_min_size();

    vscroll = std::min(0.0f, max_child_min_size.y - size.y);
}

int32_t ScrollContainer::get_vscroll() {
    return vscroll;
}

void ScrollContainer::propagate_draw(VkCommandBuffer p_command_buffer) {
    auto global_pos = get_global_position();
    auto size = get_size();
    auto dst_rect = RectF(global_pos, global_pos + size);

    auto vector_server = VectorServer::get_singleton();

    auto canvas = vector_server->get_canvas();

    // Use a RenderTarget to achieve content clip, instead of using clip path.
    if (sub_render_target.size == Vec2I()) {
        sub_render_target = Pathfinder::RenderTarget(canvas->get_driver(), size.to_i32(), "Scroller render target");
    }

    auto render_target_id = canvas->get_scene()->push_render_target(sub_render_target);

    vector_server->global_transform_offset = Transform2::from_translation(-global_pos);

    Node::propagate_draw(p_command_buffer);

    vector_server->global_transform_offset = Transform2();

    canvas->get_scene()->pop_render_target();

    vector_server->get_canvas()->draw_render_target(render_target_id, dst_rect);

    // Draw scroll bar overlay.
    draw(p_command_buffer);
}

void ScrollContainer::propagate_input(std::vector<InputEvent> &input_queue) {
    // TODO: We should also clip mouse input events as we do to drawing.
    Node::propagate_input(input_queue);
}

} // namespace Flint
