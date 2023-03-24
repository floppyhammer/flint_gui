#include "text_server.h"

namespace Flint {

uint64_t TextServer::load_font_from_file(const std::string& file_path) {
    return 0;
}

std::shared_ptr<std::vector<char>> TextServer::get_font(uint64_t font_id) {
    auto find = raw_font_cache.find(font_id);
    if (find != raw_font_cache.end()) {
        return raw_font_cache[font_id];
    }
    return nullptr;
}

void TextServer::cleanup() {
    font_cache.clear();
    raw_font_cache.clear();
}

} // namespace Flint
