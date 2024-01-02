#ifndef FLINT_TEXT_EDIT_H
#define FLINT_TEXT_EDIT_H

#include <cstdint>
#include <memory>
#include <optional>

#include "../../common/geometry.h"
#include "../../resources/style_box.h"
#include "label.h"

namespace Flint {

class MarginContainer;

/**
 * Single-Line text input field.
 */
class TextEdit : public NodeUi {
public:
    TextEdit();

    void set_text(const std::string &new_text);

    std::string get_text() const;

    void input(InputEvent &event) override;

    void update(double dt) override;

    void draw() override;

    Vec2F calc_minimum_size() const override;

    void grab_focus() override;

    void release_focus() override;

    void set_editable(bool new_value);

private:
    bool editable = true;
    bool single_line = false;

    /// The codepoint which the caret currently targets to. (Codepoint, not glyph!)
    /// -1 means the caret is at the begining (before the first codepoint if there's any).
    int32_t current_caret_index = -1;
    int32_t selected_text_caret_index_begin = -1;
    Vec2F caret_position;

    StyleLine theme_caret;
    StyleBox theme_selection_box;

    std::shared_ptr<MarginContainer> margin_container;
    std::shared_ptr<Label> label;

    float caret_blink_timer = 0;

    void delete_selection();

    void cursor_entered() override;

    void cursor_exited() override;

    int32_t calculate_caret_index(Vec2F local_cursor_position);

    Vec2F calculate_caret_position(int32_t caret_index);

    std::optional<StyleBox> theme_normal;
    std::optional<StyleBox> theme_focused;
    std::optional<StyleBox> theme_uneditable;
};

} // namespace Flint

#endif // FLINT_TEXT_EDIT_H
