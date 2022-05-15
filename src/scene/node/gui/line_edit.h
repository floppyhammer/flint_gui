#ifndef FLINT_LINE_EDIT_H
#define FLINT_LINE_EDIT_H

#include "label.h"
#include "../../../resources/style_box.h"
#include "../../../common/math/rect.h"

#include <cstdint>
#include <memory>

namespace Flint {
    class LineEdit : public Control {
    public:
        LineEdit();

        void set_text(const std::string &p_text);

        std::string get_text() const;

        void update(double dt) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        Vec2<float> calculate_minimum_size() const override;

    private:
        bool editable = true;

        std::shared_ptr<Label> label;

        std::optional<StyleBox> theme_normal;
        std::optional<StyleBox> theme_focused;
        std::optional<StyleBox> theme_uneditable;
    };
}

#endif //FLINT_LINE_EDIT_H
