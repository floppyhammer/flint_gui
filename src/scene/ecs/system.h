#ifndef FLINT_SYSTEM_H
#define FLINT_SYSTEM_H

#include <set>

#include "component.h"
#include "entity.h"

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
} // namespace Flint

#endif // FLINT_SYSTEM_H
