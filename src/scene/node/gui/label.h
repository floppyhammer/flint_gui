#ifndef FLINT_LABEL_H
#define FLINT_LABEL_H

#include "control.h"
#include "../../../resources/style_box.h"
#include "../../../resources/font.h"
#include "../../../resources/resource_manager.h"
#include "../../../common/math/rect.h"

#include <cstdint>
#include <memory>

namespace Flint {
    enum class Alignment {
        Begin,
        Center,
        End,
    };

    struct Glyph {
        int start = -1; // Start offset in the source string.
        int end = -1; // End offset in the source string.

        uint8_t count = 0; // Number of glyphs in the grapheme, set in the first glyph only.
        uint8_t repeat = 1; // Draw multiple times in the row.
        uint16_t flags = 0; // Grapheme flags (valid, rtl, virtual), set in the first glyph only.

        float x_off = 0.f; // Offset from the origin of the glyph on baseline.
        float y_off = 0.f;
        float advance = 0.f; // Advance to the next glyph along baseline(x for horizontal layout, y for vertical).

        int font_size = 0; // Font size;
        char32_t text{};
        int32_t index = 0; // Glyph index (font specific) or UTF-32 codepoint (for the invalid glyphs).

        Rect<float> layout_box;
        Rect<float> bbox;

        Pathfinder::Shape shape; // Glyph shape.
    };

    class Label : public Control {
    public:
        Label();

        /**
         * Set text context.
         * @note See https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html for glyph conventions.
         * @param p_text Text string.
         */
        void set_text(const std::string &p_text);

        std::string get_text() const;

        void insert_text(uint32_t position, const std::string &p_text);

        void remove_text(uint32_t position);

        void set_size(Vec2<float> p_size) override;

        void set_font(std::shared_ptr<Font> p_font);

        void set_text_style(float p_size, ColorU p_color, float p_stroke_width, ColorU p_stroke_color);

        void update(double dt) override;

        void draw(VkCommandBuffer p_command_buffer) override;

        bool debug = false;

        void set_horizontal_alignment(Alignment alignment);

        void set_vertical_alignment(Alignment alignment);

        Vec2<float> calculate_minimum_size() const override;

        std::vector<Glyph> &get_glyphs();

        float get_font_size() const;

    private:
        void measure();

        void consider_alignment();

        Vec2<float> get_text_size() const;

    private:
        std::wstring text;

        std::shared_ptr<Font> font;

        float font_size = 32;

        std::vector<Glyph> glyphs;

        mutable Rect<float> layout_box;

        // Fill
        ColorU color{163, 163, 163, 255};

        // Stroke
        float stroke_width = 0;
        ColorU stroke_color;

        // Layout
        Alignment horizontal_alignment = Alignment::Begin;
        Alignment vertical_alignment = Alignment::Begin;
        Vec2<float> alignment_shift{0};

        std::optional<StyleBox> theme_background;
    };
}

#endif //FLINT_LABEL_H
