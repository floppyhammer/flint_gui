#include "file_dialog.h"

#include "../common/utils.h"
#include "tinyfiledialogs/tinyfiledialogs.h"

namespace revector {

std::optional<std::string> FileDialog::show() {
    Logger::info("Show file dialog", "revector");

    // In case of multiple files, the separator is `|`.
    // Returns NULL on cancel.
    auto path = tinyfd_openFileDialog("",                    // NULL or ""
                                      default_path_.c_str(), // NULL or ""
                                      0,                     // 0 (2 in the following example)
                                      nullptr, // NULL or char const * lFilterPatterns[2]={"*.png","*.jpg"};
                                      nullptr, // NULL or "image files"
                                      0);      // 0

    if (path) {
        Logger::info("Selected path: " + std::string(path), "revector");
        return path;
    }

    return {};
}

void FileDialog::set_default_path(const std::string& default_path) {
    default_path_ = default_path;
}

} // namespace revector
