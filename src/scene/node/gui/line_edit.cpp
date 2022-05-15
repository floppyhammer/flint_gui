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

        set_text("Enter text here");
    }

    void LineEdit::set_text(const std::string &p_text) {
        label->set_text(p_text);
    }

    std::string LineEdit::get_text() const {
        return label->get_text();
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

        Control::draw(p_command_buffer);
    }

    Vec2<float> LineEdit::calculate_minimum_size() const {
        return label->calculate_minimum_size();
    }
}
