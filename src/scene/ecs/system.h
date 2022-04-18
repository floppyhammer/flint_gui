#ifndef FLINT_SYSTEM_H
#define FLINT_SYSTEM_H

#include "entity.h"
#include "component.h"

#include <set>

namespace Flint {
    /**
     * Each system can then inherit from this class which allows the System Manager
     * to keep a list of pointers to systems. Inheritance, but not virtual.
     *
     * A system is any functionality that iterates upon a list of entities
     * with a certain signature of components.
     *
     * Every system needs a list of entities, and we want some logic
     * outside of the system (in the form of a manager to maintain that list).
     */
    class System {
    public:
        std::set<Entity> entities;
    };
}

#endif //FLINT_SYSTEM_H
