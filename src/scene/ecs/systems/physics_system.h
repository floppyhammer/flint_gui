#ifndef FLINT_PHYSICS_SYSTEM_H
#define FLINT_PHYSICS_SYSTEM_H

#include "../system.h"
#include "../entity.h"
#include "../coordinator.h"

#include <set>

namespace Flint {
    class Physics2dSystem : public System {
    public:
        void update(const std::weak_ptr<Coordinator>& p_coordinator, double dt);
    };

    class Physics3dSystem : public System {
    public:
        void update(const std::weak_ptr<Coordinator>& p_coordinator, double dt);
    };
}

#endif //FLINT_PHYSICS_SYSTEM_H
