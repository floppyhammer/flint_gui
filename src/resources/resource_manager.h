#ifndef FLINT_RESOURCE_MANAGER_H
#define FLINT_RESOURCE_MANAGER_H

#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "resource.h"

namespace Flint {

class ResourceManager {
public:
    static ResourceManager *get_singleton() {
        static ResourceManager singleton;
        return &singleton;
    }

    template <typename T>
    std::shared_ptr<T> load(const std::string &path) {
        static_assert(std::is_base_of<Resource, T>::value, "T must inherit from Resource!");

        auto res = resources[path].lock();
        if (!res) {
            // assuming constructor loads resource
            resources[path] = res = std::make_shared<T>(path);
        }

        auto return_value = std::dynamic_pointer_cast<T>(res);
        if (!return_value) {
            throw std::runtime_error("Resource '" + path + "' is already loaded as another type!");
        }

        return return_value;
    }

private:
    std::unordered_map<std::string, std::weak_ptr<Resource>> resources;
};

} // namespace Flint

#endif // FLINT_RESOURCE_MANAGER_H
