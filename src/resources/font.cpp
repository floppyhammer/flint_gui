#include "font.h"

#include "../common/io.h"
#include "../common/logger.h"

#define STB_TRUETYPE_IMPLEMENTATION

#include <vector>

#include "pathfinder.h"
#include "stb_truetype.h"

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
}

Font::~Font() {
    free(buffer);
}

void Font::get_metrics(float line_height, int &ascent, int &descent, int &line_gap) {
    // Calculate font scaling.
    scale = stbtt_ScaleForPixelHeight(&info, line_height);

    stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);

    // Take scale into account.
    ascent = int(roundf(float(ascent) * scale));
    descent = int(roundf(float(descent) * scale));
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
                path.quadratic_curve_to(v.cx * scale, v.cy * -scale, v.x * scale, v.y * -scale);
            } break;
            case STBTT_vcubic: {
                path.bezier_curve_to(
                    v.cx * scale, v.cy * -scale, v.cx1 * scale, v.cy1 * -scale, v.x * scale, v.y * -scale);
            } break;
        }
    }

    // Close the last contour in the outline.
    path.close_path();

    stbtt_FreeShape(&info, vertices);

    return path;
}

int32_t Font::find_index(int codepoint) {
    return stbtt_FindGlyphIndex(&info, codepoint);
}

Rect<int> Font::get_bounds(int32_t glyph_index) {
    Rect<int> bounding_box;

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

    return advance_width * scale;
}

} // namespace Flint
