#ifndef FLINT_PHYSICS_SYSTEM_H
#define FLINT_PHYSICS_SYSTEM_H

#include "../system.h"
#include "../entity.h"

#include <set>

namespace Flint {
    class Physics2dSystem : public System {
    public:
        void update(double dt);
    };

    class Physics3dSystem : public System {
    public:
        void update(double dt);
    };
}

#endif //FLINT_PHYSICS_SYSTEM_H
