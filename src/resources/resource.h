#ifndef FLINT_RESOURCE_H
#define FLINT_RESOURCE_H

#include <cstdint>
#include <memory>
#include <string>

namespace Flint {
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
} // namespace Flint

#endif // FLINT_RESOURCE_H
