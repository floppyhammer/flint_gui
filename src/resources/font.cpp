#include "font.h"

#include <codecvt>
#include <locale>
#include <string>
#include <vector>

#include "../common/load_file.h"
#include "../common/logger.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#ifdef _WIN32
    // With Windows 10 Fall Creators Update and later, you can just include the single header <icu.h>.
    // See https://learn.microsoft.com/en-us/windows/win32/intl/international-components-for-unicode--icu-
    #include <icu.h>
#else
    // Built-in ICU data.
    #ifdef ICU_STATIC_DATA
        #include "icu4c/icudata.gen.h"
    #endif

    #include <unicode/ubidi.h>
    #include <unicode/ubrk.h>
    #include <unicode/uclean.h>
    #include <unicode/udata.h>
    #include <unicode/uscript.h>
    #include <unicode/utypes.h>
#endif

namespace Flint {

enum class Script {
    Common,
    Arabic,
    Bengali,
    Devanagari,
    Hebrew,
};

hb_script_t to_harfbuzz_script(Script script) {
    switch (script) {
        case Script::Arabic: {
            return HB_SCRIPT_ARABIC;
        }
        case Script::Hebrew: {
            return HB_SCRIPT_HEBREW;
        }
        case Script::Bengali: {
            return HB_SCRIPT_BENGALI;
        }
        case Script::Devanagari: {
            return HB_SCRIPT_DEVANAGARI;
        }
        default: {
            return HB_SCRIPT_COMMON;
        }
    }
}

Script get_text_script(const std::string &text) {
    std::wstring_convert<std::codecvt<char16_t, char, std::mbstate_t>, char16_t> convert;
    std::u16string utf16_string = convert.from_bytes(text);

    for (auto &codepoint : utf16_string) {
        if (codepoint >= 0x0600 && codepoint <= 0x06FF) {
            return Script::Arabic;
        }
        if (codepoint >= 0x0981 && codepoint <= 0x09FB) {
            return Script::Bengali;
        }
        if (codepoint >= 0x0901 && codepoint <= 0x097F) {
            return Script::Devanagari;
        }
        if (codepoint >= 0x0590 && codepoint <= 0x05FF) {
            return Script::Hebrew;
        }
    }

    return Script::Common;
}

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

void Font::get_glyphs(const std::string &text,
                      std::vector<Glyph> &glyphs,
                      std::vector<Pathfinder::Range> &para_ranges) {
    glyphs.clear();
    para_ranges.clear();

#ifdef ICU_STATIC_DATA
    static bool icu_data_loaded = false;
    if (!icu_data_loaded) {
        UErrorCode err = U_ZERO_ERROR;
        u_init(&err); // Do not check for errors, since we only load part of the data.
        icu_data_loaded = true;
    }
#else
// Load data manually.
#endif

    uint32_t units_per_em = hb_face_get_upem(harfbuzz_res->face);

    // Note: don't use icu::UnicodeString, it doesn't work. Use plain UChar* instead.

    std::wstring_convert<std::codecvt<char16_t, char, std::mbstate_t>, char16_t> convert;
    std::u16string text_u16 = convert.from_bytes(text);

    const UChar *uchar_data = text_u16.c_str();
    const int32_t uchar_count = text_u16.length();

    // Bidi for the whole text (paragraphs).
    UBiDi *para_bidi = ubidi_open();
    // Bidi for a paragraph (lines).
    // This would a child bidi of para_bidi. The order to destroy them matters.
    UBiDi *line_bidi = ubidi_open();

    UErrorCode error_code = U_ZERO_ERROR;

    do {
        // Paragraphs are seperated by line breaks.
        std::cout << "Paragraphs: " << text << std::endl;

        // Set paragraphs.
        ubidi_setPara(para_bidi, uchar_data, uchar_count, UBIDI_DEFAULT_LTR, nullptr, &error_code);
        if (!U_SUCCESS(error_code)) {
            Logger::error("ubidi_setPara() failed!", "TextServer");
            break;
        }

        int32_t para_count = ubidi_countParagraphs(para_bidi);

        // Go through paragraphs.
        for (int32_t para_index = 0; para_index < para_count; para_index++) {
            // Paragraph start and end in the whole text. Unit: u16char.
            int32_t para_start, para_end;
            UBiDiLevel para_level;
            ubidi_getParagraphByIndex(para_bidi, para_index, &para_start, &para_end, &para_level, &error_code);

            if (!U_SUCCESS(error_code)) {
                Logger::error("ubidi_getParagraphByIndex() failed!", "TextServer");
                break;
            }

            std::string para_text = convert.to_bytes(text_u16.substr(para_start, para_end));
            std::cout << "Paragraph text: " << para_text << std::endl;
            std::cout << "Paragraph range: \t" << para_start << "\t" << para_end << std::endl;

            // Set a paragraph (lines).
            ubidi_setLine(para_bidi, para_start, para_end, line_bidi, &error_code);
            if (!U_SUCCESS(error_code)) {
                Logger::error("ubidi_setLine failed!", "TextServer");
                break;
            }

            // The first glyph in the new paragraph.
            size_t para_glyph_start = glyphs.size();

            // Get run count in the current paragraph.
            int32_t run_count = ubidi_countRuns(line_bidi, &error_code);

            // Go through runs.
            for (int32_t run_index = 0; run_index < run_count; run_index++) {
                // Run start and end in the paragraph. Unit: u16char.
                int32_t logical_start, length;
                UBiDiDirection dir = ubidi_getVisualRun(line_bidi, run_index, &logical_start, &length);

                bool run_is_rtl = dir == UBIDI_RTL;

                // Get run text from the whole text.
                std::u16string run_text_u16 = text_u16.substr(para_start + logical_start, length);
                std::string run_text = convert.to_bytes(run_text_u16);

                std::cout << "Visual run in line: \t" << run_index << "\t" << run_is_rtl << "\t" << logical_start
                          << '\t' << length << '\t' << run_text << std::endl;

                auto run_script = get_text_script(run_text);

                // Buffers are sequences of Unicode characters that use the same font
                // and have the same text direction, script, and language.
                hb_buffer_t *hb_buffer = hb_buffer_create();

                // Item offset and length should represent a specific run.
                hb_buffer_add_utf16(
                    hb_buffer, reinterpret_cast<const uint16_t *>(uchar_data), -1, para_start + logical_start, length);

                hb_buffer_set_direction(hb_buffer, run_is_rtl ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
                hb_buffer_set_script(hb_buffer, to_harfbuzz_script(run_script));

                hb_shape(harfbuzz_res->font, hb_buffer, nullptr, 0);

                unsigned int glyph_count;
                hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(hb_buffer, &glyph_count);
                hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(hb_buffer, &glyph_count);

                // Shaped glyph positions will always be in one line (regardless of line breaks).
                for (int i = 0; i < glyph_count; i++) {
                    auto &info = glyph_info[i];
                    auto &pos = glyph_pos[i];

                    // Cluster unit is u16char, so it should be worked with std::u16string instead of std::string.
                    Pathfinder::Range current_cluster;
                    if (!run_is_rtl) {
                        if (i < glyph_count - 1) {
                            current_cluster = {info.cluster, glyph_info[i + 1].cluster};
                        } else {
                            current_cluster = {info.cluster, (unsigned long long)(para_start + logical_start + length)};
                        }
                    } else {
                        if (i == 0) {
                            current_cluster = {info.cluster, (unsigned long long)(para_start + logical_start + length)};
                        } else {
                            current_cluster = {info.cluster, glyph_info[i - 1].cluster};
                        }
                    }

                    std::string glyph_text =
                        convert.to_bytes(text_u16.substr(current_cluster.start, current_cluster.length()));
                    //                    std::cout << "Glyph text: " << glyph_text << std::endl;

                    // Skip line breaks, so they're not drawn.
                    if (glyph_text == "\n") {
                        continue;
                    }

                    Glyph glyph;

                    // Codepoint property is replaced with glyph ID after shaping.
                    glyph.index = info.codepoint;

                    // Check if the glyph has already been cached.
                    if (glyph_cache.find(glyph.index) != glyph_cache.end()) {
                        glyphs.push_back(glyph_cache[glyph.index]);
                        continue;
                    }

                    glyph.x_offset = pos.x_offset;
                    glyph.y_offset = pos.y_offset;

                    // Don't know why harfbuzz returns incorrect advance.
                    // So, we use the info provided by freetype.
                    //            glyph.x_advance = (float)pos.x_advance * font_size / (float)units_per_em;
                    glyph.x_advance = get_advance(glyph.index);

                    // Get glyph path.
                    glyph.path = get_glyph_path(glyph.index);

                    // The glyph's layout box in the glyph's local coordinates.
                    // The origin is the baseline. The Y axis is downward.
                    glyph.box = RectF(0, (float)-ascent, glyph.x_advance, (float)-descent);

                    // Get the glyph path's bounding box. The Y axis points down.
                    RectI bounding_box = get_bounds(glyph.index);

                    // BBox in the glyph's local coordinates.
                    glyph.bbox = bounding_box.to_f32();

                    glyphs.push_back(glyph);
                }

                hb_buffer_destroy(hb_buffer);
            }

            // Record glyph start and end in the new paragraph.
            para_ranges.emplace_back(para_glyph_start, glyphs.size());
        }
    } while (false);

    ubidi_close(line_bidi);
    ubidi_close(para_bidi);
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

uint32_t Font::get_size() const {
    return size;
}

int Font::get_ascent() const {
    return ascent;
}

int Font::get_descent() const {
    return descent;
}

} // namespace Flint
