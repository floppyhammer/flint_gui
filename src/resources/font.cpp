#include "font.h"

#include <vector>

#include "../common/load_file.h"
#include "../common/logger.h"
#include "pathfinder.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include "unicode/localpointer.h"
#include "unicode/ubidi.h"
#include "unicode/uchar.h"
#include "unicode/unistr.h"
#include "unicode/utypes.h"

using icu::UnicodeString;

namespace Flint {

Font::Font(const std::string &path) : Resource(path) {
    auto bytes = load_file_as_bytes(path.c_str());

    auto byte_size = bytes.size() * sizeof(unsigned char);

    stbtt_buffer = static_cast<unsigned char *>(malloc(byte_size));
    memcpy(stbtt_buffer, bytes.data(), byte_size);

    // Prepare font info.
    if (!stbtt_InitFont(&stbtt_info, stbtt_buffer, 0)) {
        Logger::error("Failed to prepare font info!", "Font");
    }

    get_metrics();

    harfbuzz_res = std::make_shared<HarfBuzzRes>(path);
}

Font::Font(std::vector<char> &bytes) {
    auto byte_size = bytes.size() * sizeof(unsigned char);

    stbtt_buffer = static_cast<unsigned char *>(malloc(byte_size));
    memcpy(stbtt_buffer, bytes.data(), byte_size);

    // Prepare font info.
    if (!stbtt_InitFont(&stbtt_info, stbtt_buffer, 0)) {
        Logger::error("Failed to prepare font info!", "Font");
    }

    get_metrics();
}

Font::~Font() {
    free(stbtt_buffer);
}

void Font::get_metrics() {
    // Calculate font scaling.
    scale = stbtt_ScaleForPixelHeight(&stbtt_info, size);

    // The origin is baseline and the Y axis points u.
    // So, ascent is usually positive, and descent negative.
    int unscaled_ascent;
    int unscaled_descent;
    int unscaled_line_gap;
    stbtt_GetFontVMetrics(&stbtt_info, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

    // Take scale into account.
    ascent = roundf(float(unscaled_ascent) * scale);
    descent = roundf(float(unscaled_descent) * scale);
}

Pathfinder::Path2d Font::get_glyph_path(uint16_t glyph_index) const {
    Pathfinder::Path2d path;

    stbtt_vertex *vertices = nullptr;

    int num_vertices = stbtt_GetGlyphShape(&stbtt_info, glyph_index, &vertices);

    // Glyph has no shape (e.g. Space).
    if (vertices == nullptr) {
        return path;
    }

    for (int i = 0; i < num_vertices; i++) {
        auto &v = vertices[i];

        switch (v.type) {
            case STBTT_vmove: {
                // Close the last contour in the outline (if there's any).
                path.close_path();
                path.move_to(v.x * scale, v.y * -scale);
            } break;
            case STBTT_vline: {
                path.line_to(v.x * scale, v.y * -scale);
            } break;
            case STBTT_vcurve: {
                path.quadratic_to(v.cx * scale, v.cy * -scale, v.x * scale, v.y * -scale);
            } break;
            case STBTT_vcubic: {
                path.cubic_to(v.cx * scale, v.cy * -scale, v.cx1 * scale, v.cy1 * -scale, v.x * scale, v.y * -scale);
            } break;
        }
    }

    // Close the last contour in the outline.
    path.close_path();

    stbtt_FreeShape(&stbtt_info, vertices);

    return path;
}

std::vector<Glyph> Font::get_glyphs(const std::string &text, Language lang) {
    // Buffers are sequences of Unicode characters that use the same font
    // and have the same text direction, script, and language.
    hb_buffer_t *hb_buffer = hb_buffer_create();

    uint32_t units_per_em = hb_face_get_upem(harfbuzz_res->face);

    std::vector<Glyph> glyphs;

    UnicodeString us(text.c_str());

    UBiDi *bidi = ubidi_open();

    UErrorCode error_code = U_ZERO_ERROR;

    UBiDiLevel paraLevel = UBIDI_DEFAULT_LTR;

    ubidi_setPara(bidi, us.getBuffer(), us.length(), paraLevel, nullptr, &error_code);

    if (U_SUCCESS(error_code)) {
        UnicodeString u_string(ubidi_getText(bidi));

        std::string Ustr;
        u_string.toUTF8String(Ustr);

        int32_t count = ubidi_countRuns(bidi, &error_code);
        int32_t logical_start, length;

        for (int32_t i = 0; i < count; i++) {
            UBiDiDirection dir = ubidi_getVisualRun(bidi, i, &logical_start, &length);
            std::string dir_str = "UBIDI_LTR";
            if (dir == UBIDI_RTL) {
                dir_str = "UBIDI_RTL";
            }

            std::string str55;
            UnicodeString temp = u_string.tempSubString(logical_start, length);

            temp.toUTF8String(str55);

            std::cout << "VisualRun \t" << dir_str << "\t" << logical_start << '\t' << length << '\t' << str55
                      << std::endl;
        }
    } else {
        std::cout << "Failed" << std::endl;
    }

    // TODO: should have a loop handling split text runs.
    {
        // Item offset and length should represent a specific run.
        hb_buffer_add_utf8(hb_buffer, text.c_str(), -1, 0, -1);

        // TODO: should set these based on a specific run.
        switch (lang) {
            case Language::Chinese:
            case Language::English: {
                hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR);
                hb_buffer_set_script(hb_buffer, HB_SCRIPT_LATIN);
                hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));
            } break;
            case Language::Arabic: {
                hb_buffer_set_direction(hb_buffer, HB_DIRECTION_RTL);
                hb_buffer_set_script(hb_buffer, HB_SCRIPT_ARABIC);
                hb_buffer_set_language(hb_buffer, hb_language_from_string("ar", -1));
            } break;
        }

        hb_shape(harfbuzz_res->font, hb_buffer, nullptr, 0);

        unsigned int glyph_count;
        hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(hb_buffer, &glyph_count);
        hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(hb_buffer, &glyph_count);

        // Shaped glyph positions will always be in one line (regardless of line breaks).
        for (int i = 0; i < glyph_count; i++) {
            Glyph glyph;

            auto &info = glyph_info[i];
            auto &pos = glyph_pos[i];

            // Codepoint property is replaced with glyph ID after shaping.
            glyph.index = info.codepoint;

            // Check if the glyph has already been cached.
            if (glyph_cache.find(glyph.index) != glyph_cache.end()) {
                glyphs.push_back(glyph_cache[glyph.index]);
                continue;
            }

            glyph.x_offset = pos.x_offset;
            glyph.y_offset = pos.y_offset;

            // Don't why harfbuzz returns incorrect advance.
            //            glyph.x_advance = (float)pos.x_advance * font_size / (float)units_per_em;
            glyph.x_advance = get_advance(glyph.index);

            // Get glyph path.
            glyph.path = get_glyph_path(glyph.index);

            // The glyph's layout box in the glyph's local coordinates.
            // The origin is the baseline. The Y axis is downward.
            glyph.box = RectF(0, -ascent, glyph.x_advance, -descent);

            // Get the glyph path's bounding box. The Y axis points down.
            RectI bounding_box = get_bounds(glyph.index);

            // BBox in the glyph's local coordinates.
            glyph.bbox = bounding_box.to_f32();

            glyphs.push_back(glyph);
        }
    }

    hb_buffer_destroy(hb_buffer);

    return glyphs;
}

int32_t Font::find_index(int codepoint) {
    return stbtt_FindGlyphIndex(&stbtt_info, codepoint);
}

RectI Font::get_bounds(uint16_t glyph_index) const {
    RectI bounding_box;

    stbtt_GetGlyphBitmapBox(&stbtt_info,
                            glyph_index,
                            scale,
                            scale,
                            &bounding_box.left,
                            &bounding_box.top,
                            &bounding_box.right,
                            &bounding_box.bottom);

    return bounding_box;
}

float Font::get_advance(uint16_t glyph_index) const {
    // The horizontal distance to increment (for left-to-right writing) or decrement (for right-to-left writing)
    // the pen position after a glyph has been rendered when processing text.
    // It is always positive for horizontal layouts, and zero for vertical ones.
    int advance_width;

    // The horizontal distance from the current pen position to the glyph's left bbox edge.
    // It is positive for horizontal layouts, and in most cases negative for vertical ones.
    int left_side_bearing;

    stbtt_GetGlyphHMetrics(&stbtt_info, glyph_index, &advance_width, &left_side_bearing);

    return (float)advance_width * scale;
}

void Font::set_size(uint32_t new_size) {
    if (new_size == size) {
        return;
    }

    size = new_size;

    get_metrics();
}

int Font::get_ascent() const {
    return ascent;
}

int Font::get_descent() const {
    return descent;
}

} // namespace Flint
