#ifndef FLINT_RESOURCE_H
#define FLINT_RESOURCE_H

#include <cstdint>
#include <memory>
#include <string>

class Resource {
public:
    Resource() = default;
    Resource(const std::string &path) { name = path;}

    virtual ~Resource() = default;

    // Name for debugging reason.
    std::string name{};
};

#endif //FLINT_RESOURCE_H
