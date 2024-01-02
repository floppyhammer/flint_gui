#include "text_edit.h"

#include <string>

#include "../../common/utils.h"
#include "margin_container.h"

namespace Flint {

TextEdit::TextEdit() {
    type = NodeType::TextEdit;

    label = std::make_shared<Label>();
    label->set_vertical_alignment(Alignment::Center);

    margin_container = std::make_shared<MarginContainer>();
    margin_container->set_margin_all(4);
    margin_container->add_child(label);
    margin_container->set_parent(this);
    margin_container->set_size(size);

    theme_normal = std::optional(StyleBox());
    theme_normal->bg_color = ColorU(10, 10, 10);

    theme_focused = std::optional(theme_normal.value());
    theme_focused->border_color = ColorU(163, 163, 163, 255);
    theme_focused->border_width = 2;

    theme_caret.width = 2;

    theme_selection_box.bg_color = ColorU(33, 66, 131, 255);
    theme_selection_box.border_width = 0;
    theme_selection_box.corner_radius = 0;

    set_text("Enter text");
}

void TextEdit::set_text(const std::string &new_text) {
    label->set_text(new_text);
}

std::string TextEdit::get_text() const {
    return label->get_text();
}

void TextEdit::input(InputEvent &event) {
    NodeUi::input(event);

    // Handle mouse input propagation.
    bool consume_flag = false;

    int32_t codepoint_count = label->get_text_u32().size();

    auto global_position = get_global_position();

    auto active_rect = RectF(global_position, global_position + size);

    switch (event.type) {
        case InputEventType::MouseButton: {
            auto args = event.args.mouse_button;

            if (active_rect.contains_point(args.position)) {
                if (args.pressed) {
                    // Decide caret position.
                    auto local_mouse_pos = get_local_mouse_position();
                    current_caret_index = calculate_caret_index(local_mouse_pos);
                    selected_text_caret_index_begin = current_caret_index;

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

                Logger::verbose("Caret position: current - " + std::to_string(current_caret_index) + ", selected - " +
                                    std::to_string(selected_text_caret_index_begin),
                                "TextEdit");
            }
        } break;
        case InputEventType::Text: {
            if (!focused) {
                break;
            }

            if (current_caret_index < codepoint_count) {
                if (selected_text_caret_index_begin != current_caret_index) {
                    delete_selection();
                }

                if (editable) {
                    label->insert_text(current_caret_index + 1, cpp11_codepoint_to_utf8(event.args.text.codepoint));
                }

                current_caret_index++;
                selected_text_caret_index_begin = current_caret_index;
                caret_blink_timer = 0;
            }

            consume_flag = true;
        } break;
        case InputEventType::Key: {
            auto key_args = event.args.key;

            if (key_args.key == KeyCode::Backspace && current_caret_index > -1) {
                if (key_args.pressed || key_args.repeated) {
                    if (selected_text_caret_index_begin != current_caret_index) {
                        delete_selection();
                    } else {
                        if (editable) {
                            label->remove_text(current_caret_index, 1);
                        }

                        current_caret_index--;
                        selected_text_caret_index_begin--;
                    }
                    caret_blink_timer = 0;
                }
            }

            if (key_args.pressed || key_args.repeated) {
                if (key_args.key == KeyCode::Left && current_caret_index > -1) {
                    current_caret_index--;
                    selected_text_caret_index_begin--;
                    caret_blink_timer = 0;
                } else if (key_args.key == KeyCode::Right && current_caret_index < codepoint_count - 1) {
                    current_caret_index++;
                    selected_text_caret_index_begin++;
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

void TextEdit::update(double dt) {
    NodeUi::update(dt);

    caret_blink_timer += dt;

    margin_container->propagate_update(dt);
}

void TextEdit::draw() {
    auto vector_server = VectorServer::get_singleton();

    auto global_position = get_global_position();

    // Draw bg.
    {
        std::optional<StyleBox> active_style_box;
        active_style_box = focused ? theme_focused : theme_normal;

        if (active_style_box.has_value()) {
            vector_server->draw_style_box(active_style_box.value(), global_position, size);
        }
    }

    // Draw selection box.
    if (focused) {
        if (selected_text_caret_index_begin != current_caret_index) {
            auto start = calculate_caret_position(std::min(current_caret_index, selected_text_caret_index_begin));
            auto end = calculate_caret_position(std::max(current_caret_index, selected_text_caret_index_begin));
            auto box_position = label->get_global_position() + start;
            auto box_size = Vec2F(0, label->get_font()->get_size()) + (end - start);
            vector_server->draw_style_box(theme_selection_box, box_position, box_size);
        }
    }

    label->set_size(size);

    // Draw text.
    label->draw();

    // Draw blinking caret.
    if (focused && editable) {
        theme_caret.color.a_ = 255.0f * std::ceil(std::sin(caret_blink_timer * 5.0f));

        float current_glyph_right_edge = 0;
        if (current_caret_index > -1 && current_caret_index < label->get_glyphs().size()) {
            current_glyph_right_edge = label->get_glyph_right_edge_position(current_caret_index);
        }
        if (current_caret_index == -1) {
            current_glyph_right_edge = label->get_glyph_right_edge_position(current_caret_index);
        }

        auto start = label->get_global_position() + Vec2F(current_glyph_right_edge, 3);
        auto end = start + Vec2F(0, label->get_font()->get_size() - 6);
        vector_server->draw_style_line(theme_caret, start, end);
    }

    NodeUi::draw();
}

Vec2F TextEdit::calc_minimum_size() const {
    auto container_size = margin_container->calc_minimum_size();

    return container_size.max(minimum_size);
}

int32_t TextEdit::calculate_caret_index(Vec2F local_cursor_position) {
    auto closest_codepoint_index = -1;
    auto closest_distance = std::numeric_limits<float>::max();
    const auto &codepoints = label->get_text_u32();

    for (int i = 0; i < codepoints.size(); i++) {
        float codepoint_right_edge = label->get_codepoint_right_edge_position(i);

        // Mouse position to the right boundary of the glyph.
        auto distance = abs(local_cursor_position.x - codepoint_right_edge);

        if (distance < closest_distance) {
            closest_distance = distance;
            closest_codepoint_index = i;
        }
    }

    if (codepoints.empty()) {
        float first_codepoint_center =
            label->get_codepoint_left_edge_position(0) + label->get_codepoint_right_edge_position(0);
        if (local_cursor_position.x < first_codepoint_center) {
            return -1;
        }
    }

    // Caret at the beginning of the text, i.e. before the first glyph.
    if (abs(local_cursor_position.x) < closest_distance) {
        closest_codepoint_index = -1;
    }

    return closest_codepoint_index;
}

Vec2F TextEdit::calculate_caret_position(int32_t target_caret_index) {
    auto closest_glyph_index = -1;
    auto closest_distance = std::numeric_limits<float>::max();

    if (target_caret_index > -1) {
        return {label->get_glyph_right_edge_position(target_caret_index), 0};
    } else {
        return {0, 0};
    }
}

void TextEdit::grab_focus() {
    focused = true;
}

void TextEdit::release_focus() {
    focused = false;
}

void TextEdit::set_editable(bool new_value) {
    editable = new_value;
}

void TextEdit::cursor_entered() {
    // InputServer::get_singleton()->set_cursor(get_window()->get_real().get(), CursorShape::IBeam);
}

void TextEdit::cursor_exited() {
    // InputServer::get_singleton()->set_cursor(get_window()->get_real().get(), CursorShape::Arrow);
}

void TextEdit::delete_selection() {
    if (!editable) {
        return;
    }
    auto start_index = std::min(selected_text_caret_index_begin, current_caret_index) + 1;
    auto count = std::abs(selected_text_caret_index_begin - current_caret_index);
    label->remove_text(start_index, count);
    current_caret_index = selected_text_caret_index_begin = start_index - 1;
}

} // namespace Flint
