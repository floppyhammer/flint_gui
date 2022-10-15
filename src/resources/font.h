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

    void set_size(uint32_t new_font_size);

    Pathfinder::Path2d get_glyph_path(int glyph_index) const;

    int32_t find_index(int codepoint);

    float get_advance(int32_t glyph_index);

    Rect<int> get_bounds(int32_t glyph_index);

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

} // namespace Flint

#endif // FLINT_FONT_H
