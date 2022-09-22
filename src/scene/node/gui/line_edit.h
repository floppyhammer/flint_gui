#ifndef FLINT_LINE_EDIT_H
#define FLINT_LINE_EDIT_H

#include <cstdint>
#include <memory>

#include "../../../common/geometry.h"
#include "../../../resources/style_box.h"
#include "label.h"

namespace Flint {
/**
 * Single-Line text input field.
 */
class LineEdit : public Control {
public:
    LineEdit();

    void set_text(const std::string &p_text);

    std::string get_text() const;

    void input(std::vector<InputEvent> &input_queue) override;

    void update(double dt) override;

    void draw(VkCommandBuffer p_command_buffer) override;

    Vec2<float> calculate_minimum_size() const override;

    void grab_focus() override;

    void release_focus() override;

private:
    bool editable = true;

    int32_t selected_caret_index = -1;
    int32_t current_caret_index = -1;
    Vec2F caret_position;

    StyleLine theme_caret;
    StyleBox theme_selection_box;

    std::shared_ptr<Label> label;

    float caret_blink_timer = 0;

    void cursor_entered() override;

    void cursor_exited() override;

    int32_t calculate_caret_index(Vec2F local_cursor_position);

    Vec2F calculate_caret_position(int32_t caret_index);

    std::optional<StyleBox> theme_normal;
    std::optional<StyleBox> theme_focused;
    std::optional<StyleBox> theme_uneditable;
};
} // namespace Flint

#endif // FLINT_LINE_EDIT_H
