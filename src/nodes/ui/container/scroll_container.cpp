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
    // Get the minimum size.
    Vec2F max_child_min_size = get_max_child_min_size();

    auto min_size = max_child_min_size.max(custom_minimum_size);

    // Adjust own size.

    if (!vscroll_enabled) {
        size.y = size.max(min_size).y;
    }
    if (!hscroll_enabled) {
        size.x = size.max(min_size).x;
    }

    // Adjust child size.
    for (auto &child : children) {
        if (child->is_ui_node()) {
            auto cast_child = dynamic_cast<NodeUi *>(child.get());
            cast_child->set_position({0, 0});

            if (!vscroll_enabled) {
                cast_child->set_size({cast_child->get_size().x, size.y});
            }
            if (!hscroll_enabled) {
                cast_child->set_size({size.x, cast_child->get_size().y});
            }
        }
    }
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
                    } else {
                        vscroll -= delta * scroll_speed;
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
    NodeUi::update(dt);

    adjust_layout();

    if (children.empty() || !children.front()->is_ui_node()) {
        return;
    }

    vscroll = std::max(0.0f, vscroll);
    hscroll = std::max(0.0f, hscroll);

    Vec2F max_child_min_size = get_max_child_min_size();

    if (max_child_min_size.x > size.x) {
        hscroll = std::min(hscroll, max_child_min_size.x - size.x);
    } else {
        hscroll = 0;
    }

    if (max_child_min_size.y > size.y) {
        vscroll = std::min(vscroll, max_child_min_size.y - size.y);
    } else {
        hscroll = 0;
    }

    // Scroll container can only have one effective control child.
    auto content = (NodeUi *)children.front().get();

    content->set_position({-hscroll, -vscroll});
}

void ScrollContainer::draw_scroll_bar() {
    if (children.empty() || !children.front()->is_ui_node()) {
        return;
    }

    if (!visible_) {
        return;
    }

    // Scroll container can only have one effective control child.
    auto content = (NodeUi *)children.front().get();
    auto content_size = content->get_size();

    auto vector_server = VectorServer::get_singleton();

    auto global_pos = get_global_position();
    auto size = get_size();

    // Vertical.
    if (content_size.y > size.y) {
        auto scroll_bar_pos = Vec2F(size.x - 8, 0) + global_pos;
        auto scroll_bar_size = Vec2F(8, size.y);

        vector_server->draw_style_box(theme_scroll_bar, scroll_bar_pos, scroll_bar_size);

        auto grabber_length = size.y / content_size.y * size.y;

        auto grabber_pos = Vec2F(size.x - 8, size.y / content_size.y * vscroll) + global_pos;
        auto grabber_size = Vec2F(8, grabber_length);

        vector_server->draw_style_box(theme_scroll_grabber, grabber_pos, grabber_size);
    }

    // Horizontal.
    if (content_size.x > size.x) {
        auto scroll_bar_pos = Vec2F(0, size.y - 8) + global_pos;
        auto scroll_bar_size = Vec2F(size.x, 8);

        vector_server->draw_style_box(theme_scroll_bar, scroll_bar_pos, scroll_bar_size);

        auto grabber_length = size.x / content_size.x * size.x;

        auto grabber_pos = Vec2F(size.x / content_size.x * hscroll, size.y - 8) + global_pos;
        auto grabber_size = Vec2F(grabber_length, 8);

        vector_server->draw_style_box(theme_scroll_grabber, grabber_pos, grabber_size);
    }

    NodeUi::draw();
}

void ScrollContainer::set_hscroll(int32_t value) {
    if (children.empty()) {
        return;
    }

    hscroll = value;
}

int32_t ScrollContainer::get_hscroll() const {
    return hscroll;
}

void ScrollContainer::set_vscroll(int32_t value) {
    if (children.empty()) {
        return;
    }

    vscroll = value;
}

int32_t ScrollContainer::get_vscroll() const {
    return vscroll;
}

void ScrollContainer::pre_draw_children() {
    if (!visible_) {
        return;
    }

    auto global_pos = get_global_position();
    auto size = get_size() * get_window()->get_dpi_scaling_factor();

    auto vector_server = VectorServer::get_singleton();
    vector_server->set_render_layer(render_layer);

    auto canvas = vector_server->get_canvas();

    // Use a RenderTarget to achieve content clip, instead of using clip path.
    if (render_target_desc.size == Vec2I()) {
        render_target_desc = {size.to_i32(), "ScrollContainer render target"};
    }

    // Draw all children onto the temporary render target.
    temp_draw_data.render_target_id = canvas->get_scene()->push_render_target(render_target_desc);

    // For the temporary render target, we need to offset all child nodes back to the origin.
    vector_server->global_transform_offset = Transform2::from_translation(-global_pos);
}

void ScrollContainer::post_draw_children() {
    if (!visible_) {
        return;
    }

    auto global_pos = get_global_position();
    auto size = get_size();

    auto vector_server = VectorServer::get_singleton();

    auto canvas = vector_server->get_canvas();

    draw_scroll_bar();

    vector_server->global_transform_offset = Transform2();

    // Don't draw on the temporary render target anymore.
    canvas->get_scene()->pop_render_target();

    float dpi_scale = get_window()->get_dpi_scaling_factor();

    auto dst_rect = RectF(global_pos * dpi_scale, (global_pos + size) * dpi_scale);
    vector_server->get_canvas()->draw_render_target(temp_draw_data.render_target_id, dst_rect);

    vector_server->set_render_layer(0);
}

} // namespace Flint
