#ifndef FLINT_FONT_H
#define FLINT_FONT_H

#include <hb.h>
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

struct HarfBuzzRes {
    hb_blob_t *blob{}; /* or hb_blob_create_from_file_or_fail() */
    hb_face_t *face{};
    hb_font_t *font{};

    HarfBuzzRes() = default;

    HarfBuzzRes(const std::string &filename) {
        blob = hb_blob_create_from_file(filename.c_str()); /* or hb_blob_create_from_file_or_fail() */
        face = hb_face_create(blob, 0);
        font = hb_font_create(face);
        hb_font_set_ptem(font, 32);

        int x_scale;
        int y_scale;
        hb_font_get_scale(font, &x_scale, &y_scale);
        //        hb_font_set_scale(font, x_scale / 3.0, y_scale / 3.0);
    }

    ~HarfBuzzRes() {
        if (font) {
            hb_font_destroy(font);
        }
        if (face) {
            hb_face_destroy(face);
        }
        if (blob) {
            hb_blob_destroy(blob);
        }
    }
};

// TODO: languages should be auto-detected.
enum class Language {
    Chinese,
    English,
    Arabic,
};

struct Glyph {
    int start = -1; // Start offset in the source string.
    int end = -1;   // End offset in the source string.

    uint8_t repeat = 0; // Draw multiple times in the row.

    float x_offset = 0; // Offset from the origin of the glyph on baseline.
    float y_offset = 0;
    float x_advance = 0; // Advance to the next glyph along baseline (x for horizontal layout, y for vertical).
    float y_advance = 0;

    int32_t font_size = 0; // Font size;
    char32_t text{};

    // Glyph index (font specific) or UTF-32 codepoint (for the invalid glyphs).
    // A particular glyph ID within the font does not necessarily correlate to a predictable Unicode codepoint.
    int32_t index = 0;

    // Glyph path. The points are in the glyph's baseline coordinates.
    Pathfinder::Path2d path;

    /// Glyph box in the baseline coordinates.
    RectF box;

    /// Glyph path's bounding box in the baseline coordinates.
    RectF bbox;

    /// Layout box in the whole text. Coordinate origin is the top-left corner.
    RectF layout_box;
};

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

    void get_glyphs_harfbuzz(const std::string &text, Language lang, std::vector<Glyph> &glyphs) const;

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

    std::shared_ptr<HarfBuzzRes> harfbuzz_res;

    void get_metrics();
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
