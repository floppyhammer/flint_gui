#pragma once

#include <pathfinder/prelude.h>

#include <vector>

#define FTR(TAG) revector::TranslationServer::get_singleton()->get_translation(TAG)

namespace revector {

class TranslationServer {
public:
    static TranslationServer *get_singleton() {
        static TranslationServer singleton;
        return &singleton;
    }

    TranslationServer();

    void set_locale(const std::string &locale);

    std::string get_translation(std::string tag);

    void load_translations(const std::string& filename);

private:
    std::map<std::string, std::map<std::string, std::string>> db_;

    std::string current_locale_ = "en";
};

} // namespace revector
