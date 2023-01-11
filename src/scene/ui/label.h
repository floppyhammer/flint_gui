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
    Label(const std::string &_text);

    /**
     * Set text context.
     * @note See https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html for glyph conventions.
     * @param p_text Text string.
     */
    void set_text(const std::string &new_text);

    std::string get_text() const;

    void insert_text(uint32_t position, const std::string &new_text);

    void remove_text(uint32_t position, uint32_t count);

    void set_size(Vec2F p_size) override;

    void set_font(std::shared_ptr<Font> p_font);

    void set_text_style(float p_size, ColorU p_color, float p_stroke_width, ColorU p_stroke_color);

    void update(double dt) override;

    void draw() override;

    void set_horizontal_alignment(Alignment alignment);

    void set_vertical_alignment(Alignment alignment);

    Vec2F calc_minimum_size() const override;

    std::vector<Glyph> &get_glyphs();

    float get_font_size() const;

    void set_language(Language new_lang);

    StyleBox theme_background;

    float get_glyph_position(uint32_t glyph_index);

private:
    void measure();

    void consider_alignment();

    Vec2F get_text_size() const;

private:
    std::string text;

    std::wstring text_debug;

    Language language = Language::English;

    std::shared_ptr<Font> font;

    bool clip = false;

    float font_size = 32;

    // Layout-independent.
    std::vector<Glyph> glyphs;

    // Layout-dependent.
    std::vector<Vec2F> glyph_positions;

    mutable RectF layout_box;

    FontStyle font_style;
    // Fill
    ColorU color{163, 163, 163, 255};

    // Stroke
    float stroke_width = 0;
    ColorU stroke_color;

    // Layout
    Alignment horizontal_alignment = Alignment::Begin;
    Alignment vertical_alignment = Alignment::Begin;
    Vec2F alignment_shift{0};
};

} // namespace Flint

#endif // FLINT_LABEL_H
