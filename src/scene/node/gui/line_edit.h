#ifndef FLINT_LINE_EDIT_H
#define FLINT_LINE_EDIT_H

#include "label.h"
#include "../../../resources/style_box.h"
#include "../../../common/geometry.h"

#include <cstdint>
#include <memory>

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

        int32_t caret_index = -1;
        Vec2F caret_position;

        StyleLine theme_caret;

        std::shared_ptr<Label> label;

        std::optional<StyleBox> theme_normal;
        std::optional<StyleBox> theme_focused;
        std::optional<StyleBox> theme_uneditable;
    };
}

#endif //FLINT_LINE_EDIT_H
