#include "scroll_container.h"

using Pathfinder::clamp;

namespace Flint {

ScrollContainer::ScrollContainer() {
    type = NodeType::ScrollContainer;

    theme_scroll_bar.bg_color = ColorU(100, 100, 100, 50);
    theme_scroll_bar.corner_radius = 8;

    theme_scroll_grabber.bg_color = ColorU(163, 163, 163, 255);
    theme_scroll_grabber.corner_radius = 8;
}

void ScrollContainer::adjust_layout() {
    // Do nothing.
}

void ScrollContainer::calc_minimum_size() {
    calculated_minimum_size = custom_minimum_size;
}

void ScrollContainer::input(InputEvent &event) {
    NodeUi::input(event);

    if (children.empty() || !children.front()->is_ui_node()) {
        return;
    }

    // Scroll container can only have one effective control child.
    auto content = (NodeUi *)children.front().get();

    auto global_position = get_global_position();

    auto active_rect = RectF(global_position, global_position + size);

    // Handle mouse input propagation.
    bool consume_flag = false;

    switch (event.type) {
        case InputEventType::MouseScroll: {
            float delta = event.args.mouse_scroll.y_delta;

            if (active_rect.contains_point(InputServer::get_singleton()->cursor_position)) {
                if (!event.is_consumed()) {
                    if (InputServer::get_singleton()->is_key_pressed(KeyCode::LeftShift)) {
                        hscroll -= delta * scroll_speed;

                        auto grabber_length = size.x / content->get_size().x * size.x;
                        hscroll = clamp(hscroll, 0, int32_t(size.x - grabber_length));
                    } else {
                        vscroll -= delta * scroll_speed;

                        auto grabber_length = size.y / content->get_size().y * size.y;
                        vscroll = clamp(vscroll, 0, int32_t(size.y - grabber_length));
                    }
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

void ScrollContainer::update(double dt) {
    if (children.empty() || !children.front()->is_ui_node()) {
        return;
    }

    // Scroll container can only have one effective control child.
    auto content = (NodeUi *)children.front().get();

    content->set_position({(float)-hscroll, (float)-vscroll});

    NodeUi::update(dt);
}

void ScrollContainer::draw_scroll_bar() {
    if (children.empty() || !children.front()->is_ui_node()) {
        return;
    }

    // Scroll container can only have one effective control child.
    auto content = (NodeUi *)children.front().get();
    auto content_size = content->get_size();

    auto vector_server = VectorServer::get_singleton();

    auto global_pos = get_global_position();
    auto size = get_size();

    // Vertical.
    {
        auto scroll_bar_pos = Vec2F(size.x - 8, 0) + global_pos;
        auto scroll_bar_size = Vec2F(8, size.y);

        vector_server->draw_style_box(theme_scroll_bar, scroll_bar_pos, scroll_bar_size);

        auto grabber_pos = Vec2F(size.x - 8, vscroll) + global_pos;
        auto grabber_size = Vec2F(8, size.y / content_size.y * size.y);

        vector_server->draw_style_box(theme_scroll_grabber, grabber_pos, grabber_size);
    }

    // Horizontal.
    {
        auto scroll_bar_pos = Vec2F(0, size.y - 8) + global_pos;
        auto scroll_bar_size = Vec2F(size.x, 8);

        vector_server->draw_style_box(theme_scroll_bar, scroll_bar_pos, scroll_bar_size);

        auto grabber_pos = Vec2F(hscroll, size.y - 8) + global_pos;
        auto grabber_size = Vec2F(size.x / content_size.x * size.x, 8);

        vector_server->draw_style_box(theme_scroll_grabber, grabber_pos, grabber_size);
    }

    NodeUi::draw();
}

void ScrollContainer::set_hscroll(int32_t value) {
    if (children.empty()) {
        return;
    }

    Vec2F max_child_min_size = get_max_child_min_size();

    hscroll = std::min(0.0f, max_child_min_size.x - size.x);
}

int32_t ScrollContainer::get_hscroll() const {
    return hscroll;
}

void ScrollContainer::set_vscroll(int32_t value) {
    if (children.empty()) {
        return;
    }

    Vec2F max_child_min_size = get_max_child_min_size();

    vscroll = std::min(0.0f, max_child_min_size.y - size.y);
    hscroll = std::min(0.0f, max_child_min_size.x - size.x);
}

int32_t ScrollContainer::get_vscroll() const {
    return vscroll;
}

void ScrollContainer::pre_draw_children() {
    auto global_pos = get_global_position();
    auto size = get_size();

    auto vector_server = VectorServer::get_singleton();

    auto canvas = vector_server->get_canvas();

    // Use a RenderTarget to achieve content clip, instead of using clip path.
    if (render_target_desc.size == Vec2I()) {
        render_target_desc = {size.to_i32(), "Scroller render target"};
    }

    temp_draw_data.render_target_id = canvas->get_scene()->push_render_target(render_target_desc);

    vector_server->global_transform_offset = Transform2::from_translation(-global_pos);
}

void ScrollContainer::post_draw_children() {
    auto global_pos = get_global_position();
    auto size = get_size();

    auto vector_server = VectorServer::get_singleton();

    auto canvas = vector_server->get_canvas();

    draw_scroll_bar();

    vector_server->global_transform_offset = Transform2();

    canvas->get_scene()->pop_render_target();

    auto dst_rect = RectF(global_pos, global_pos + size);
    vector_server->get_canvas()->draw_render_target(temp_draw_data.render_target_id, dst_rect);
}

} // namespace Flint
