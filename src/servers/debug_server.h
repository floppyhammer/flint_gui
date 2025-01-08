#ifndef FLINT_DEBUG_SERVER_H
#define FLINT_DEBUG_SERVER_H

#include <map>
#include <string>

namespace Flint {
class DebugServer final {
public:
    static DebugServer *get_singleton() {
        static DebugServer singleton;
        return &singleton;
    }

    std::map<std::string, bool> debug_flags;
};

}

#endif //FLINT_DEBUG_SERVER_H
