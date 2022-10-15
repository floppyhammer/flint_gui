#ifndef FLINT_FONT_H
#define FLINT_FONT_H

#include <pathfinder.h>
#include <stb_truetype.h>

#include <cstdio>
#include <cstdlib>

#include "pathfinder.h"
#include "resource.h"

using Pathfinder::Rect;

namespace Flint {

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

    stbtt_fontinfo info{};

    void get_metrics(float line_height, int &ascent, int &descent, int &line_gap);

    Pathfinder::Path2d get_glyph_path(int glyph_index) const;

    int32_t Font::find_index(int codepoint);

    float get_advance(int32_t glyph_index);

    Rect<int> get_bounds(int32_t glyph_index);

private:
    /// Stores font data, should not be freed until font is deleted.
    unsigned char *buffer;

    float scale;
};

} // namespace Flint

#endif // FLINT_FONT_H
