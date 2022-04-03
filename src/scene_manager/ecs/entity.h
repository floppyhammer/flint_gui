//
// Created by floppyhammer on 4/2/2022.
//

#ifndef FLINT_ENTITY_H
#define FLINT_ENTITY_H

#include <cstdint>

namespace Flint {
    // A simple type alias
    using Entity = std::uint32_t;

    // Used to define the size of arrays later on
    const Entity MAX_ENTITIES = 5000;
}

#endif //FLINT_ENTITY_H
