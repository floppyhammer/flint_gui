#include "file_dialog.h"

namespace Flint {
std::string FileDialog::show() {
    Logger::info("File dialog", "FileDialog");

    // In case of multiple files, the separator is `|`.
    // Returns NULL on cancel.
    auto path = tinyfd_openFileDialog("",      // NULL or ""
                                      "",      // NULL or ""
                                      0,       // 0 (2 in the following example)
                                      nullptr, // NULL or char const * lFilterPatterns[2]={"*.png","*.jpg"};
                                      nullptr, // NULL or "image files"
                                      0);      // 0

    if (path) {
        Logger::info("File dialog selected: " + std::string(path), "FileDialog");
        return path;
    }

    return "";
}
} // namespace Flint
