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

enum class Alignment {
    Begin,
    Center,
    End,
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

    void set_size(Vec2F new_size) override;

    void set_font(std::shared_ptr<Font> new_font);

    void set_text_style(TextStyle new_text_style);

    void update(double dt) override;

    void draw() override;

    void set_horizontal_alignment(Alignment alignment);

    void set_vertical_alignment(Alignment alignment);

    Vec2F calc_minimum_size() const override;

    std::vector<Glyph> &get_glyphs();

    std::shared_ptr<Font> get_font() const;

    float get_glyph_left_edge_position(int32_t glyph_index);

    float get_glyph_right_edge_position(int32_t glyph_index);

    float get_codepoint_left_edge_position(int32_t codepoint_index);

    float get_codepoint_right_edge_position(int32_t codepoint_index);

    /// Get the caret position of a given codepoint index.
    float get_position_by_codepoint(uint32_t codepoint_index);

    /// Get the closest codepoint to a mouse click.
    uint32_t get_codepoint_by_position(Vec2F position);

public:
    StyleBox theme_background;

private:
    void measure();

    void consider_alignment();

    Vec2F get_text_size() const;

private:
    // Raw text.
    std::string text_;
    // Codepoint separated text.
    std::u32string text_u32_;

    /// text_u32_ is more related to navigation, glyphs_ is more about rendering.

    std::shared_ptr<Font> font, emoji_font;

    bool clip = false;

    bool autowrap = false;

    // Layout-independent. Glyph count will not necessarily be the same as the character count.
    std::vector<Glyph> glyphs_;

    // Layout-dependent. Ranges for glyphs, not for characters.
    std::vector<Pathfinder::Range> para_ranges;
    std::vector<Pathfinder::Range> line_ranges;

    // Layout-dependent.
    std::vector<Vec2F> glyph_positions;

    mutable RectF layout_box;

    std::vector<RectF> glyph_boxes;
    std::vector<RectF> character_boxes;

    bool need_to_remeasure = true;
    bool need_to_update_layout = true;

    TextStyle text_style;

    // Layout
    Alignment horizontal_alignment = Alignment::Begin;
    Alignment vertical_alignment = Alignment::Begin;
    Vec2F alignment_shift{0};
};

} // namespace Flint

#endif // FLINT_LABEL_H
