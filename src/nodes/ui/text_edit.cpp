#include "text_edit.h"

#include <string>

#include "../../common/utils.h"
#include "../../servers/input_server.h"
#include "container/margin_container.h"

namespace Flint {

TextEdit::TextEdit() {
    type = NodeType::TextEdit;

    label = std::make_shared<Label>();
    label->set_vertical_alignment(Alignment::Center);
    label->set_mouse_filter(MouseFilter::Ignore);

    margin_container = std::make_shared<MarginContainer>();
    margin_container->set_margin_all(4);
    margin_container->add_child(label);
    margin_container->set_mouse_filter(MouseFilter::Ignore);

    add_embedded_child(margin_container);

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

    callbacks_cursor_entered.emplace_back(
        [this] { InputServer::get_singleton()->set_cursor(this->get_window(), CursorShape::IBeam); });

    callbacks_cursor_exited.emplace_back(
        [this] { InputServer::get_singleton()->set_cursor(this->get_window(), CursorShape::Arrow); });
}

void TextEdit::set_text(const std::string &new_text) {
    label->set_text(new_text);
}

std::string TextEdit::get_text() const {
    return label->get_text();
}

void TextEdit::input(InputEvent &event) {
    auto input_server = InputServer::get_singleton();

    auto window = get_window();

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
                    // Decide which codepoint the caret is at.
                    auto local_mouse_pos_to_label = args.position - label->get_global_position();
                    current_caret_index = calculate_caret_index(local_mouse_pos_to_label);
                    selection_start_index = current_caret_index;

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

                Logger::verbose("Caret position: current " + std::to_string(current_caret_index) + ", selected " +
                                    std::to_string(selection_start_index),
                                "TextEdit");
            }
        } break;
        case InputEventType::Text: {
            if (!focused) {
                break;
            }

            if (editable) {
                if (selection_start_index != current_caret_index) {
                    delete_selection();
                }

                label->insert_text(current_caret_index, cpp11_codepoint_to_utf8(event.args.text.codepoint));

                current_caret_index++;
                selection_start_index = current_caret_index;
                caret_blink_timer = 0;
            }

            consume_flag = true;
        } break;
        case InputEventType::Key: {
            auto key_args = event.args.key;

            if (key_args.key == KeyCode::Backspace) {
                if (key_args.pressed || key_args.repeated) {
                    if (selection_start_index != current_caret_index) {
                        delete_selection();
                    } else {
                        if (editable) {
                            if (current_caret_index > 0) {
                                label->remove_text(current_caret_index - 1, 1);

                                current_caret_index--;
                                selection_start_index--;
                            }
                        }
                    }
                    caret_blink_timer = 0;
                }
            }

            if (key_args.pressed || key_args.repeated) {
                if (key_args.key == KeyCode::Left && current_caret_index > 0) {
                    current_caret_index--;
                    selection_start_index--;
                    caret_blink_timer = 0;
                } else if (key_args.key == KeyCode::Right && current_caret_index < codepoint_count) {
                    current_caret_index++;
                    selection_start_index++;
                    caret_blink_timer = 0;
                }

                if (key_args.key == KeyCode::C && input_server->is_key_pressed(KeyCode::LeftControl)) {
                    auto start_index = std::min(selection_start_index, current_caret_index);
                    auto count = std::abs((int)selection_start_index - (int)current_caret_index);
                    std::string selected_text = label->get_sub_text(start_index, count);
                    input_server->set_clipboard(get_window(), selected_text.c_str());
                }

                if (key_args.key == KeyCode::V && input_server->is_key_pressed(KeyCode::LeftControl)) {
                    auto clipboard_text = input_server->get_clipboard(get_window());
                    std::u32string clipboard_text_u32;
                    from_utf8_to_utf16(clipboard_text, clipboard_text_u32);
                    label->insert_text(current_caret_index, clipboard_text);
                    current_caret_index += clipboard_text_u32.size();
                    selection_start_index = current_caret_index;
                }

                if (key_args.key == KeyCode::X && input_server->is_key_pressed(KeyCode::LeftControl)) {
                    auto start_index = std::min(selection_start_index, current_caret_index);
                    auto count = std::abs((int)selection_start_index - (int)current_caret_index);
                    std::string selected_text = label->get_sub_text(start_index, count);
                    input_server->set_clipboard(get_window(), selected_text.c_str());
                    delete_selection();
                }
            }
        } break;
        default:
            break;
    }

    NodeUi::input(event);

    if (consume_flag) {
        event.consume();
    }
}

void TextEdit::update(double dt) {
    NodeUi::update(dt);

    margin_container->set_size(size);

    caret_blink_timer += dt;
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
        if (selection_start_index != current_caret_index) {
            auto start = calculate_caret_position(std::min(current_caret_index, selection_start_index));
            auto end = calculate_caret_position(std::max(current_caret_index, selection_start_index));
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

        float current_codepoint_right_edge = 0;
        if (current_caret_index > 0) {
            current_codepoint_right_edge = label->get_codepoint_right_edge_position(current_caret_index - 1);
        } else {
            current_codepoint_right_edge = label->get_codepoint_left_edge_position(0);
        }

        auto start = label->get_global_position() + Vec2F(current_codepoint_right_edge, 3);
        auto end = start + Vec2F(0, label->get_font()->get_size() - 6);
        vector_server->draw_style_line(theme_caret, start, end);
    }

    NodeUi::draw();
}

void TextEdit::calc_minimum_size() {
    calculated_minimum_size = margin_container->get_effective_minimum_size();
}

uint32_t TextEdit::calculate_caret_index(Vec2F local_cursor_position_to_label) {
    uint32_t closest_codepoint_index = 0;
    float closest_distance = std::numeric_limits<float>::max();

    const auto &codepoints = label->get_text_u32();

    for (int i = 0; i < codepoints.size(); i++) {
        float codepoint_right_edge = label->get_codepoint_right_edge_position(i);

        // Cursor position to the right boundary of the codepoint.
        auto distance = abs(local_cursor_position_to_label.x - codepoint_right_edge);

        if (distance < closest_distance) {
            closest_distance = distance;
            closest_codepoint_index = i;
        }
    }

    if (codepoints.empty()) {
        return 0;
    }

    // Caret is before the first codepoint.
    if (abs(local_cursor_position_to_label.x) < closest_distance) {
        return 0;
    }

    return closest_codepoint_index + 1;
}

Vec2F TextEdit::calculate_caret_position(int32_t target_caret_index) {
    if (target_caret_index > 0) {
        return {label->get_codepoint_right_edge_position(target_caret_index - 1), 0};
    }

    return {0, 0};
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

void TextEdit::delete_selection() {
    if (!editable) {
        return;
    }
    auto start_index = std::min(selection_start_index, current_caret_index);
    auto count = std::abs((int)selection_start_index - (int)current_caret_index);
    label->remove_text(start_index, count);
    current_caret_index = selection_start_index = start_index;
}

} // namespace Flint
