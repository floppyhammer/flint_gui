#include "font.h"

#include "../common/logger.h"
#include "../common/io.h"

#define STB_TRUETYPE_IMPLEMENTATION

#include "stb_truetype.h"

#include "pathfinder.h"

#include <vector>

namespace Flint {
    Font::Font(const std::string &path) {
        auto bytes = load_file_as_bytes(path.c_str());

        auto byte_size = bytes.size() * sizeof(unsigned char);
        buffer = static_cast<unsigned char *>(malloc(byte_size));
        memcpy(buffer, bytes.data(), byte_size);

        // Prepare font info.
        if (!stbtt_InitFont(&info, buffer, 0)) {
            Logger::error("Failed to prepare font info!", "Font");
        }
    }

    Font::Font(std::vector<char> &bytes) {
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

    float Font::get_metrics(float line_height, int &ascent, int &descent, int &line_gap) const {
        // Calculate font scaling.
        float scale = stbtt_ScaleForPixelHeight(&info, line_height);

        stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);

        // Take scale into account.
        ascent = int(roundf(float(ascent) * scale));
        descent = int(roundf(float(descent) * scale));

        return scale;
    }

    Pathfinder::Outline Font::get_glyph_outline(int glyph_index) const {
        Pathfinder::Outline outline;

        stbtt_vertex *vertices = nullptr;

        int num_vertices = stbtt_GetGlyphShape(&info, glyph_index, &vertices);

        // Glyph has no shape (e.g. Space).
        if (vertices == nullptr) {
            return outline;
        }

        for (int i = 0; i < num_vertices; i++) {
            auto &v = vertices[i];

            switch (v.type) {
                case STBTT_vmove: {
                    // Close last path (if there's any).
                    outline.close();

                    outline.move_to(v.x, v.y);
                }
                    break;
                case STBTT_vline: {
                    outline.line_to(v.x, v.y);
                }
                    break;
                case STBTT_vcurve: {
                    outline.curve_to(v.cx, v.cy, v.x, v.y);

                }
                    break;
                case STBTT_vcubic: {
                    outline.cubic_to(v.cx, v.cy, v.cx1, v.cy1, v.x, v.y);

                }
                    break;
            }
        }

        // Close the last contour.
        outline.close();

        stbtt_FreeShape(&info, vertices);

        return outline;
    }
}
