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
                    auto text = codepoint_to_utf8(event.args.text.codepoint);
                    std::cout << text << std::endl;

                    label->set_text(label->get_text() + text);

                    consume_flag = true;
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
        auto canvas = VectorServer::get_singleton().canvas;

        auto global_position = get_global_position();

        if (theme_normal.has_value()) {
            theme_normal.value().add_to_canvas(global_position, size, canvas);
        }

        label->draw(p_command_buffer);

        // Blink caret.
        theme_caret.color.a = 255 * std::ceil(std::sin(Engine::getSingleton().get_elapsed() * 5.0));

        auto glyph_box = Rect<float>({0, 0}, {0, label->get_font_size()});
        if (!label->get_glyphs().empty()) {
            glyph_box = label->get_glyphs()[caret_index].layout_box;
        }

        auto start = label->get_global_position() + Vec2F(glyph_box.min_x(), 0);
        auto end = start + Vec2F(0, label->get_font_size());
        theme_caret.add_to_canvas(start, end, canvas);

        Control::draw(p_command_buffer);
    }

    Vec2<float> LineEdit::calculate_minimum_size() const {
        return label->calculate_minimum_size();
    }
}
