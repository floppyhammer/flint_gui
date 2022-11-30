#ifndef FLINT_FONT_H
#define FLINT_FONT_H

#include <pathfinder.h>
#include <stb_truetype.h>

#include <cstdio>
#include <cstdlib>

#include "pathfinder.h"
#include "resource.h"

using Pathfinder::ColorU;
using Pathfinder::RectF;
using Pathfinder::RectI;
using Pathfinder::Vec2F;

namespace Flint {

/// Static font properties.
class Font : public Resource {
public:
    explicit Font(const std::string &path);

    explicit Font(std::vector<char> &bytes);

    ~Font() override;

    static std::shared_ptr<Font> from_file(const char *file_path) {
        std::shared_ptr<Font> font;

        auto font_buffer = Pathfinder::load_file_as_bytes(file_path);

        font = std::make_shared<Font>(font_buffer);

        return font;
    }

    void set_size(uint32_t new_font_size);

    Pathfinder::Path2d get_glyph_path(int glyph_index) const;

    int32_t find_index(int codepoint);

    float get_advance(int32_t glyph_index);

    RectI get_bounds(int32_t glyph_index);

    int get_ascent() const;

    int get_descent() const;

private:
    /// Stores font data, should not be freed until font is deleted.
    unsigned char *buffer;

    stbtt_fontinfo info{};

    uint32_t font_size = 32;

    float scale;

    int ascent;
    int descent;

    void get_metrics();
};

struct Glyph {
    int start = -1; // Start offset in the source string.
    int end = -1;   // End offset in the source string.

    uint8_t count = 0;  // Number of glyphs in the grapheme, set in the first glyph only.
    uint8_t repeat = 1; // Draw multiple times in the row.
    uint16_t flags = 0; // Grapheme flags (valid, rtl, virtual), set in the first glyph only.

    float x_off = 0.f; // Offset from the origin of the glyph on baseline.
    float y_off = 0.f;
    float advance = 0.f; // Advance to the next glyph along baseline (x for horizontal layout, y for vertical).

    Vec2F position;

    int font_size = 0; // Font size;
    char32_t text{};
    int32_t index = 0; // Glyph index (font specific) or UTF-32 codepoint (for the invalid glyphs).

    Pathfinder::Path2d path; // Glyph path.

    /// Glyph box in the baseline coordinates.
    RectF box;

    /// Glyph path's bounding box in the baseline coordinates.
    RectF bbox;

    /// Layout box in the text.
    RectF layout_box;
};

/// Dynamic font properties.
struct FontStyle {
    ColorU color = ColorU::white();
    ColorU stroke_color;
    float stroke_width = 0;
    bool debug = false;
};

} // namespace Flint

#endif // FLINT_FONT_H
