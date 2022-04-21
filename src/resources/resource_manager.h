#ifndef FLINT_RESOURCE_MANAGER_H
#define FLINT_RESOURCE_MANAGER_H

#include "resource.h"

#include <unordered_map>
#include <memory>

class ResourceManager {
public:
    std::unordered_map<uint64_t, std::shared_ptr<Resource>> map;
};

#endif //FLINT_RESOURCE_MANAGER_H
