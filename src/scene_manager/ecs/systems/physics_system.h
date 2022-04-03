#ifndef FLINT_PHYSICS_SYSTEM_H
#define FLINT_PHYSICS_SYSTEM_H

#include "../system.h"
#include "../entity.h"

#include <set>

namespace Flint {
    class PhysicsSystem : public System {
    public:
        void update(float dt);
    };
}

#endif //FLINT_PHYSICS_SYSTEM_H
