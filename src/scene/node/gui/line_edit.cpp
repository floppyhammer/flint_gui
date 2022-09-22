#include "line_edit.h"

#include <string>

#include "label.h"

namespace Flint {
Flint::LineEdit::LineEdit() {
    type = NodeType::LineEdit;

    label = std::make_shared<Label>("");
    label->set_parent(this);

    theme_normal = std::optional(StyleBox());
    theme_normal->bg_color = ColorU(10, 10, 10);

    theme_focused = std::optional(theme_normal.value());
    theme_focused->border_color = ColorU(163, 163, 163, 255);
    theme_focused->border_width = 2;

    theme_caret.width = 2;

    theme_selection_box.bg_color = ColorU(33, 66, 131, 255);
    theme_selection_box.border_width = 0;
    theme_selection_box.corner_radius = 0;

    set_text("Enter text here");
}

void LineEdit::set_text(const std::string &p_text) {
    label->set_text(p_text);
}

std::string LineEdit::get_text() const {
    return label->get_text();
}

void LineEdit::input(std::vector<InputEvent> &input_queue) {
    Control::input(input_queue);

    // Handle mouse input propagation.
    for (auto &event : input_queue) {
        bool consume_flag = false;

        auto &glyphs = label->get_glyphs();
        int32_t glyph_count = glyphs.size();

        auto global_position = get_global_position();
        auto active_rect = Rect<float>(global_position, global_position + size);

        switch (event.type) {
            case InputEventType::MouseButton: {
                auto args = event.args.mouse_button;

                if (active_rect.contains_point(args.position)) {
                    if (args.pressed) {
                        // Decide caret position.
                        auto local_mouse_pos = get_local_mouse_position();
                        current_caret_index = calculate_caret_index(local_mouse_pos);
                        selected_caret_index = current_caret_index;

                        is_pressed_inside = true;
                    } else {
                        is_pressed_inside = false;
                    }
                    consume_flag = true;
                }

                if (!args.pressed) {
                    is_pressed_inside = false;
                }
            } break;
            case InputEventType::MouseMotion: {
                auto args = event.args.mouse_motion;

                if (active_rect.contains_point(args.position)) {
                    consume_flag = true;
                }

                if (is_pressed_inside) {
                    current_caret_index = calculate_caret_index(get_local_mouse_position());
                    caret_blink_timer = 0;
                }
            } break;
            case InputEventType::Text: {
                if (!focused) continue;

                if (current_caret_index < (int32_t)glyph_count) {
                    label->insert_text(current_caret_index + 1, cpp11_codepoint_to_utf8(event.args.text.codepoint));
                    current_caret_index++;
                    caret_blink_timer = 0;
                }

                consume_flag = true;
            } break;
            case InputEventType::Key: {
                auto key_args = event.args.key;

                if (key_args.key == KeyCode::BACKSPACE && current_caret_index > -1) {
                    if (key_args.pressed || key_args.repeated) {
                        if (selected_caret_index != current_caret_index) {
                            auto start_index = std::min(selected_caret_index, current_caret_index) + 1;
                            auto count = std::abs(selected_caret_index - current_caret_index);
                            label->remove_text(start_index, count);
                            current_caret_index = selected_caret_index;
                        } else {
                            label->remove_text(current_caret_index, 1);
                            current_caret_index--;
                            selected_caret_index--;
                        }
                        caret_blink_timer = 0;
                    }
                }

                if (key_args.pressed || key_args.repeated) {
                    if (key_args.key == KeyCode::LEFT && current_caret_index > -1) {
                        current_caret_index--;
                        selected_caret_index--;
                        caret_blink_timer = 0;
                    } else if (key_args.key == KeyCode::RIGHT && current_caret_index < glyph_count - 1) {
                        current_caret_index++;
                        selected_caret_index++;
                        caret_blink_timer = 0;
                    }
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

void LineEdit::update(double dt) {
    Control::update(dt);

    caret_blink_timer += dt;

    label->update(dt);
}

void LineEdit::draw(VkCommandBuffer p_command_buffer) {
    auto canvas = VectorServer::get_singleton()->canvas;

    auto global_position = get_global_position();

    // Draw bg.
    {
        std::optional<StyleBox> active_style_box;
        active_style_box = focused ? theme_focused : theme_normal;

        if (active_style_box.has_value()) {
            active_style_box.value().add_to_canvas(global_position, size, canvas);
        }
    }

    // Draw selection box.
    if (focused) {
        if (selected_caret_index != current_caret_index) {
            auto start = calculate_caret_position(std::min(current_caret_index, selected_caret_index));
            auto end = calculate_caret_position(std::max(current_caret_index, selected_caret_index));
            auto box_position = label->get_global_position() + start;
            auto box_size = Vec2F(0, label->get_font_size()) + (end - start);
            theme_selection_box.add_to_canvas(box_position, box_size, canvas);
        }
    }

    label->set_size(size);

    // Draw text.
    label->draw(p_command_buffer);

    // Draw blinking caret.
    if (focused) {
        theme_caret.color.a = 255.0f * std::ceil(std::sin(caret_blink_timer * 5.0f));

        auto current_glyph_box = Rect<float>({0, 0}, {0, label->get_font_size()});
        if (current_caret_index > -1 && current_caret_index < label->get_glyphs().size()) {
            current_glyph_box = label->get_glyphs()[current_caret_index].layout_box;
        }

        auto caret_offset = current_glyph_box.max_x();

        auto start = label->get_global_position() + Vec2F(caret_offset, 3);
        auto end = start + Vec2F(0, label->get_font_size() - 6);
        theme_caret.add_to_canvas(start, end, canvas);
    }

    Control::draw(p_command_buffer);
}

Vec2<float> LineEdit::calculate_minimum_size() const {
    return label->calculate_minimum_size();
}

int32_t LineEdit::calculate_caret_index(Vec2F local_cursor_position) {
    auto closest_glyph_index = -1;
    auto closest_distance = std::numeric_limits<float>::max();
    auto &glyphs = label->get_glyphs();

    for (int i = 0; i < glyphs.size(); i++) {
        auto glyph_box = glyphs[i].layout_box;

        // Mouse position to the right boundary of the glyph.
        auto distance = abs(local_cursor_position.x - glyph_box.max_x());

        if (distance < closest_distance) {
            closest_distance = distance;
            closest_glyph_index = i;
        }
    }

    // Caret at the beginning of the text, i.e. before the first glyph.
    if (abs(local_cursor_position.x) < closest_distance) {
        closest_glyph_index = -1;
    }

    return closest_glyph_index;
}

Vec2F LineEdit::calculate_caret_position(int32_t target_caret_index) {
    auto closest_glyph_index = -1;
    auto closest_distance = std::numeric_limits<float>::max();
    auto &glyphs = label->get_glyphs();

    if (target_caret_index > -1) {
        return glyphs[target_caret_index].layout_box.upper_right();
    } else {
        return {0, 0};
    }
}

void LineEdit::grab_focus() {
    focused = true;
}

void LineEdit::release_focus() {
    focused = false;
}

void LineEdit::cursor_entered() {
    InputServer::get_singleton()->set_cursor(CursorShape::IBeam);
}

void LineEdit::cursor_exited() {
    InputServer::get_singleton()->set_cursor(CursorShape::Arrow);
}
} // namespace Flint
