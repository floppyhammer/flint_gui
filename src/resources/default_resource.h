#ifndef FLINT_DEFAULT_RESOURCE_H
#define FLINT_DEFAULT_RESOURCE_H

#include "theme.h"

namespace Flint {

class DefaultResource {
public:
    DefaultResource() {
        default_theme = std::make_shared<Theme>();
    }

    static DefaultResource *get_singleton() {
        static DefaultResource singleton;
        return &singleton;
    }

    std::shared_ptr<Theme> get_default_theme() {
        return default_theme;
    }

private:
    std::shared_ptr<Theme> default_theme;
};

} // namespace Flint

#endif // FLINT_DEFAULT_RESOURCE_H
