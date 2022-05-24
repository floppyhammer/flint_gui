#include "label.h"
#include "line_edit.h"

#include <string>

namespace Flint {
    Flint::LineEdit::LineEdit() {
        type = NodeType::LineEdit;

        label = std::make_shared<Label>();
        label->set_parent(this);

        StyleBox normal;
        normal.border_color = ColorU(163, 163, 163, 255);
        normal.border_width = 2;
        theme_normal = std::optional(normal);

        theme_caret.width = 2;

        set_text("Enter text here");
    }

    void LineEdit::set_text(const std::string &p_text) {
        label->set_text(p_text);
    }

    std::string LineEdit::get_text() const {
        return label->get_text();
    }

    void LineEdit::input(std::vector<InputEvent> &input_queue) {
        // Handle mouse input propagation.
        for (auto &event : input_queue) {
            bool consume_flag = false;

            switch (event.type) {
                case InputEventType::Text: {
                    if (!focused) continue;

                    auto &glyphs = label->get_glyphs();
                    auto glyph_count = glyphs.size();

                    if (caret_index < (int32_t) glyph_count) {
                        label->insert_text(caret_index + 1, cpp11_codepoint_to_utf8(event.args.text.codepoint));
                        caret_index++;
                    }

                    consume_flag = true;
                }
                    break;
                case InputEventType::Key: {
                    auto key_args = event.args.key;

                    if (key_args.key == KeyCode::BACKSPACE && key_args.pressed && caret_index > -1) {
                        label->remove_text(caret_index);
                        caret_index--;
                    }
                }
                    break;
                default:
                    break;
            }

            if (consume_flag) {
                event.consume();
            }
        }

        Control::input(input_queue);
    }

    void LineEdit::update(double dt) {
        Control::update(dt);

        label->update(dt);
    }

    void LineEdit::draw(VkCommandBuffer p_command_buffer) {
        auto canvas = VectorServer::get_singleton()->canvas;

        auto global_position = get_global_position();

        if (theme_normal.has_value()) {
            theme_normal.value().add_to_canvas(global_position, size, canvas);
        }

        label->draw(p_command_buffer);

        // Blink caret.
        if (focused) {
            theme_caret.color.a = 255 * std::ceil(std::sin(Engine::getSingleton()->get_elapsed() * 5.0));

            auto current_glyph_box = Rect<float>({0, 0}, {0, label->get_font_size()});
            if (caret_index > -1 && caret_index < label->get_glyphs().size()) {
                current_glyph_box = label->get_glyphs()[caret_index].layout_box;
            }

            auto caret_offset = current_glyph_box.max_x();

            auto start = label->get_global_position() + Vec2F(caret_offset, 0);
            auto end = start + Vec2F(0, label->get_font_size());
            theme_caret.add_to_canvas(start, end, canvas);
        }

        Control::draw(p_command_buffer);
    }

    Vec2<float> LineEdit::calculate_minimum_size() const {
        return label->calculate_minimum_size();
    }

    void LineEdit::grab_focus() {
        focused = true;

        // Decide caret position.
        auto local_mouse_pos = get_local_mouse_position();
        auto closest_glyph_index = -1;
        auto closest_distance = std::numeric_limits<float>::max();
        auto &glyphs = label->get_glyphs();

        for (int i = 0; i < glyphs.size(); i++) {
            auto glyph_box = glyphs[i].layout_box;

            // Mouse position to the right boundary of the glyph.
            auto distance = abs(local_mouse_pos.x - glyph_box.max_x());

            if (distance < closest_distance) {
                closest_distance = distance;
                closest_glyph_index = i;
            }
        }

        caret_index = closest_glyph_index;

        // Caret at the beginning of the text, i.e. before the first glyph.
        if (abs(local_mouse_pos.x) < closest_distance) {
            caret_index = -1;
        }
    }

    void LineEdit::release_focus() {
        focused = false;
    }
}
