#include "font.h"

#include <vector>

#include "../common/load_file.h"
#include "../common/logger.h"
#include "pathfinder.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace Flint {

Font::Font(const std::string &path) : Resource(path) {
    auto bytes = load_file_as_bytes(path.c_str());

    auto byte_size = bytes.size() * sizeof(unsigned char);

    buffer = static_cast<unsigned char *>(malloc(byte_size));
    memcpy(buffer, bytes.data(), byte_size);

    // Prepare font info.
    if (!stbtt_InitFont(&info, buffer, 0)) {
        Logger::error("Failed to prepare font info!", "Font");
    }

    get_metrics();

    harfbuzz_res = std::make_shared<HarfBuzzRes>(path);
}

Font::Font(std::vector<char> &bytes) {
    auto byte_size = bytes.size() * sizeof(unsigned char);

    buffer = static_cast<unsigned char *>(malloc(byte_size));
    memcpy(buffer, bytes.data(), byte_size);

    // Prepare font info.
    if (!stbtt_InitFont(&info, buffer, 0)) {
        Logger::error("Failed to prepare font info!", "Font");
    }

    get_metrics();
}

Font::~Font() {
    free(buffer);
}

void Font::get_metrics() {
    // Calculate font scaling.
    scale = stbtt_ScaleForPixelHeight(&info, font_size);

    // The origin is baseline and the Y axis points u.
    // So, ascent is usually positive, and descent negative.
    int unscaled_ascent;
    int unscaled_descent;
    int unscaled_line_gap;
    stbtt_GetFontVMetrics(&info, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

    // Take scale into account.
    ascent = roundf(float(unscaled_ascent) * scale);
    descent = roundf(float(unscaled_descent) * scale);
}

Pathfinder::Path2d Font::get_glyph_path(int glyph_index) const {
    Pathfinder::Path2d path;

    stbtt_vertex *vertices = nullptr;

    int num_vertices = stbtt_GetGlyphShape(&info, glyph_index, &vertices);

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

    stbtt_FreeShape(&info, vertices);

    return path;
}

void Font::get_glyphs_harfbuzz(const std::string &text, Language lang, std::vector<Glyph> &glyphs) const {
    hb_buffer_t *buf;

    // Buffers are sequences of Unicode characters that use the same font and have the same text direction, script, and
    // language.
    buf = hb_buffer_create();

    // TODO: should have a loop handling split text runs.
    {
        // Item offset and length should represent a specific run.
        hb_buffer_add_utf8(buf, text.c_str(), -1, 0, -1);

        // TODO: should set these based on a specific run.
        switch (lang) {
            case Language::Chinese:
            case Language::English: {
                hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
                hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
                hb_buffer_set_language(buf, hb_language_from_string("en", -1));
            } break;
            case Language::Arabic: {
                hb_buffer_set_direction(buf, HB_DIRECTION_RTL);
                hb_buffer_set_script(buf, HB_SCRIPT_ARABIC);
                hb_buffer_set_language(buf, hb_language_from_string("ar", -1));
            } break;
        }

        hb_shape(harfbuzz_res->font, buf, nullptr, 0);

        unsigned int glyph_count;
        hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
        hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

        glyphs.clear();

        // Output position will always be in one line (regardless of line breaks).
        for (unsigned int i = 0; i < glyph_count; i++) {
            Glyph glyph;

            // Codepoint property is replaced with glyph ID after shaping.
            glyph.index = glyph_info[i].codepoint;

            glyph.x_offset = glyph_pos[i].x_offset / font_size;
            glyph.y_offset = glyph_pos[i].y_offset / font_size;

            glyph.x_advance = glyph_pos[i].x_advance / font_size;
            glyph.y_advance = glyph_pos[i].y_advance / font_size;

            glyphs.push_back(glyph);
        }
    }

    hb_buffer_destroy(buf);
}

int32_t Font::find_index(int codepoint) {
    return stbtt_FindGlyphIndex(&info, codepoint);
}

RectI Font::get_bounds(int32_t glyph_index) {
    RectI bounding_box;

    stbtt_GetGlyphBitmapBox(&info,
                            glyph_index,
                            scale,
                            scale,
                            &bounding_box.left,
                            &bounding_box.top,
                            &bounding_box.right,
                            &bounding_box.bottom);

    return bounding_box;
}

float Font::get_advance(int32_t glyph_index) {
    // The horizontal distance to increment (for left-to-right writing) or decrement (for right-to-left writing)
    // the pen position after a glyph has been rendered when processing text.
    // It is always positive for horizontal layouts, and zero for vertical ones.
    int advance_width;

    // The horizontal distance from the current pen position to the glyph's left bbox edge.
    // It is positive for horizontal layouts, and in most cases negative for vertical ones.
    int left_side_bearing;

    stbtt_GetGlyphHMetrics(&info, glyph_index, &advance_width, &left_side_bearing);

    return (float)advance_width * scale;
}

void Font::set_size(uint32_t new_font_size) {
    if (new_font_size == font_size) {
        return;
    }

    font_size = new_font_size;

    get_metrics();
}

int Font::get_ascent() const {
    return ascent;
}

int Font::get_descent() const {
    return descent;
}

} // namespace Flint
