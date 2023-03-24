#ifndef FLINT_TEXT_SERVER_H
#define FLINT_TEXT_SERVER_H

#include <string>
#include <unordered_map>

#include "../resources/font.h"

namespace Flint {

class TextServer {
public:
    uint64_t load_font_from_file(const std::string& file_path);

    std::shared_ptr<std::vector<char>> get_font(uint64_t font_id);

    void cleanup();

private:
    std::string clipboard;

    // Cache for raw font data
    std::unordered_map<uint64_t, std::shared_ptr<std::vector<char>>> raw_font_cache;

    std::unordered_map<uint64_t, std::shared_ptr<Font>> font_cache;
};

} // namespace Flint

#endif // FLINT_TEXT_SERVER_H
