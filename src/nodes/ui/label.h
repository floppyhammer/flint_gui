#ifndef FLINT_LABEL_H
#define FLINT_LABEL_H

#include <cstdint>
#include <memory>

#include "../../common/geometry.h"
#include "../../resources/font.h"
#include "../../resources/resource_manager.h"
#include "../../resources/style_box.h"
#include "node_ui.h"

using Pathfinder::Rect;

namespace Flint {

enum class BidiAlignment {
    Auto,
    Begin,
    Center,
    End,
};

enum class Alignment {
    Begin,
    Center,
    End,
};

struct LayoutGlyph {
    Glyph glyph_;
    bool line_breakable_ = false;
};

class Label : public NodeUi {
public:
    Label();

    /**
     * Set text context.
     * @note See https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html for glyph conventions.
     * @param new_text Text string.
     */
    void set_text(const std::string &new_text);

    std::string get_text() const;

    std::u32string get_text_u32() const;

    void insert_text(uint32_t codepint_position, const std::string &new_text);

    void remove_text(uint32_t codepint_position, uint32_t count);

    std::string get_sub_text(uint32_t codepint_position, uint32_t count) const;

    void set_size(Vec2F new_size) override;

    void set_font(std::shared_ptr<Font> new_font);

    void set_font_size(uint32_t new_font_size) {
        font_size_ = new_font_size;
    }

    uint32_t get_font_size() const {
        return font_size_;
    }

    void set_text_style(TextStyle new_text_style);

    void update(double dt) override;

    void draw() override;

    void set_horizontal_alignment(Alignment alignment);

    void set_vertical_alignment(Alignment alignment);

    void calc_minimum_size() override;

    std::vector<Glyph> &get_glyphs();

    std::shared_ptr<Font> get_font() const;

    float get_glyph_left_edge_position(int32_t glyph_index);

    float get_glyph_right_edge_position(int32_t glyph_index);

    /// Get the caret position of a given codepoint index.
    float get_codepoint_right_edge_position(int32_t codepoint_index);

    bool get_word_wrap() const {
        return word_wrap_;
    }

    void set_word_wrap(bool word_wrap) {
        word_wrap_ = word_wrap;
    }

    void set_multi_line(bool enabled) {
        multi_line_ = enabled;
    }

public:
    StyleBox theme_background;

private:
    void measure();

    void make_layout();

    void consider_alignment();

    /// The minimum size of the text box, which is determined by the text content.
    Vec2F get_text_minimum_size() const;

private:
    // Raw text.
    std::string text_;
    // Codepoint separated text.
    std::u32string text_u32_;

    /// text_u32_ is more related to navigation, glyphs_ is more about rendering.

    std::shared_ptr<Font> font, emoji_font;

    uint32_t font_size_;

    bool clip = false;

    bool multi_line_ = false;

    /// If automatically break lines at suitable positions.
    bool word_wrap_ = false;

    // Layout-independent. Glyph count will not necessarily be the same as the character count.
    std::vector<Glyph> glyphs_;

    std::vector<LayoutGlyph> layout_glyphs_;

    // Layout-dependent. Ranges for glyphs, not for characters.
    std::vector<Line> paragraphs_;

    // If word_wrap is enabled, use this instead of para_ranges.
    std::vector<Line> lines_;

    // Layout-dependent.
    std::vector<Vec2F> glyph_positions;

    mutable RectF layout_box;

    std::vector<RectF> glyph_boxes;
    std::vector<RectF> character_boxes;

    bool need_to_remeasure = true;
    bool need_to_update_layout = true;

    TextStyle text_style;

    // Controls how to align the text box to the label area.
    Alignment horizontal_alignment = Alignment::Begin;
    Alignment vertical_alignment = Alignment::Begin;
    Vec2F alignment_shift{0};

    BidiAlignment bidi_alignment_ = BidiAlignment::Auto;
};

} // namespace Flint

#endif // FLINT_LABEL_H
