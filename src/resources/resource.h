#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace revector {

class Resource {
public:
    Resource() = default;

    explicit Resource(const std::string &path) {
        name = path;
    }

    virtual ~Resource() = default;

    // Name for debugging reason.
    std::string name{};
};

} // namespace revector
