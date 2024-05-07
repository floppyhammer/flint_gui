#ifndef FLINT_FONT_H
#define FLINT_FONT_H

#include <hb.h>
#include <pathfinder/prelude.h>
#include <stb/stb_truetype.h>

#include <codecvt>
#include <cstdio>
#include <cstdlib>
#include <locale>

#include "../common/geometry.h"
#include "../common/utils.h"
#include "resource.h"

namespace Flint {

template <typename T>
void utf8_to_utf16(const std::string &source, std::basic_string<T> &result) {
    std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
    result = convertor.from_bytes(source);

    if (convertor.converted() < source.size()) {
        throw std::runtime_error("Incomplete utf16-to-utf8 conversion!");
    }
}

template <typename T>
std::string utf16_to_utf8(const std::basic_string<T> &source) {
    std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
    std::string result = convertor.to_bytes(source);

    if (convertor.converted() < source.size()) {
        throw std::runtime_error("Incomplete utf8-to-utf16 conversion!");
    }

    return result;
}

template <typename T>
void utf8_to_utf32(const std::string &source, std::basic_string<T> &result) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, T> convertor;
    result = convertor.from_bytes(source);

    if (convertor.converted() < source.size()) {
        throw std::runtime_error("Incomplete utf8-to-utf32 conversion!");
    }
}

template <typename T>
std::string utf32_to_utf8(const std::basic_string<T> &source) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, T> convertor;
    std::string result = convertor.to_bytes(source);

    if (convertor.converted() < source.size()) {
        throw std::runtime_error("Incomplete utf32-to-utf8 conversion!");
    }

    return result;
}

// // This should convert to whatever the system-wide character encoding
// // is for the platform (UTF-32/Linux - UCS-2/Windows)
// std::string ws_to_utf8(std::wstring const &s) {
//     std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cnv;
//     std::string utf8 = cnv.to_bytes(s);
//     if (cnv.converted() < s.size()) {
//         throw std::runtime_error("Incomplete wstring-to-utf8 conversion!");
//     }
//     return utf8;
// }
//
// std::wstring utf8_to_ws(std::string const &utf8) {
//     std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cnv;
//     std::wstring s = cnv.from_bytes(utf8);
//     if (cnv.converted() < utf8.size()) {
//         throw std::runtime_error("Incomplete utf8-to-wstring conversion!");
//     }
//     return s;
// }

struct HarfBuzzRes {
    hb_blob_t *blob{}; /* or hb_blob_create_from_file_or_fail() */
    hb_face_t *face{};
    hb_font_t *font{};

    HarfBuzzRes() = default;

    explicit HarfBuzzRes(const std::vector<char> &bytes) {
        blob = hb_blob_create(bytes.data(),
                              bytes.size(),
                              HB_MEMORY_MODE_READONLY,
                              nullptr,
                              nullptr); /* or hb_blob_create_from_file_or_fail() */
        face = hb_face_create(blob, 0);
        font = hb_font_create(face);
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

struct TextStyle {
    ColorU color = ColorU::white();
    ColorU stroke_color;
    float stroke_width = 0;
    bool italic = false;
    bool bold = false;
    bool debug = false;
};

enum class Script {
    Common,
    Arabic,
    Bengali,
    Devanagari,
    Hebrew,
    Cjk,
};

// Text-context-independent glyph data.
struct Glyph {
    // Glyph index (font specific) or UTF-32 codepoint (for the invalid glyphs).
    // A particular glyph ID within the font does not necessarily correlate to a predictable Unicode codepoint.
    uint16_t index = 0;

    std::u32string codepoints;

    std::string text;

    bool emoji = false;

    Script script = Script::Common;

    int32_t x_offset = 0; // Offset from the origin of the glyph on baseline.
    int32_t y_offset = 0;

    float x_advance = 0; // Advance to the next glyph along baseline (x for horizontal layout, y for vertical).
    float y_advance = 0;

    bool skip_drawing = false;

    // Glyph path. The points are in the glyph's baseline coordinates.
    Pathfinder::Path2d path;

    // Only emojis have SVG data.
    //
    // The points' origin is not top-left (like normal SVG images) but the font baseline,
    // so the points don't fall in the view box specified by the image.
    // Therefore, we need to pass an appropriate transform when appending the SVG scene.
    std::string svg;

    /// Glyph box in the baseline coordinates, which has nothing to do with the glyph position in the text paragraph.
    RectF box;

    /// Glyph path's bounding box in the baseline coordinates, which has nothing to do with the glyph position in the
    /// text paragraph.
    RectF bbox;
};

struct Line {
    Pathfinder::Range glyph_ranges;
    bool rtl = false;
    float width = 0;
};

class Font : public Resource {
public:
    explicit Font(const std::string &path);

    ~Font() override;

    void set_size(uint32_t new_size);

    uint32_t get_size() const;

    Pathfinder::Path2d get_glyph_path(uint16_t glyph_index) const;

    std::string get_glyph_svg(uint16_t glyph_index) const;

    /// Paragraphs and lines are different concepts.
    /// Paragraphs are seperated by line breaks, while lines are results of automatic layout.
    /// A paragraph may contain one or more lines.
    void get_glyphs(const std::string &text, std::vector<Glyph> &glyphs, std::vector<Line> &para_ranges);

    uint16_t find_glyph_index_by_codepoint(int codepoint);

    float get_glyph_advance(uint16_t glyph_index) const;

    RectI get_glyph_bounds(uint16_t glyph_index) const;

    int get_ascent() const;

    int get_descent() const;

private:
    /// Stores font data, should not be freed until font is deleted.
    unsigned char *stbtt_buffer{};

    stbtt_fontinfo stbtt_info{};

    uint32_t size = 32;

    float scale;

    int32_t ascent;
    int32_t descent;

    std::shared_ptr<HarfBuzzRes> harfbuzz_res;

    std::unordered_map<uint16_t, Glyph> glyph_cache;

    std::vector<char> font_data;

private:
    void get_metrics();
};

} // namespace Flint

#endif // FLINT_FONT_H
