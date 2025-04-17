#pragma once

#include <map>
#include <string>

namespace revector {
class DebugServer final {
public:
    static DebugServer *get_singleton() {
        static DebugServer singleton;
        return &singleton;
    }

    std::map<std::string, bool> debug_flags;
};

}
