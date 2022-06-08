#ifndef FLINT_FONT_H
#define FLINT_FONT_H

#include "resource.h"

#include <stb_truetype.h>
#include <pathfinder.h>

#include <cstdio>
#include <cstdlib>

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

        float get_metrics(float line_height, int &ascent, int &descent, int &line_gap) const;

        Pathfinder::Outline get_glyph_outline(int glyph_index) const;

    private:
        /// Stores font data, should not be freed until font is deleted.
        unsigned char *buffer;
    };
}

#endif //FLINT_FONT_H
