#pragma once

#include <pathfinder/prelude.h>

#include <vector>

#define FTR(TAG) Flint::TranslationServer::get_singleton()->get_translation(TAG)

namespace Flint {

class TranslationServer {
public:
    static TranslationServer *get_singleton() {
        static TranslationServer singleton;
        return &singleton;
    }

    TranslationServer();

    void set_locale(const std::string &locale);

    std::string get_translation(std::string tag);

    void load_translations(std::string filename);

private:
    std::map<std::string, std::map<std::string, std::string>> db_;

    std::string current_locale_ = "en";
};

} // namespace Flint
