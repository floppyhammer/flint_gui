#ifndef HIERARCHY_SYSTEM_H
#define HIERARCHY_SYSTEM_H

#include "../system.h"
#include "../entity.h"
#include "../coordinator.h"

namespace Flint {
    class HierarchySystem : public System {
    public:
        void propagate(const std::weak_ptr<Coordinator>& p_coordinator, Entity root);
    };
}

#endif //HIERARCHY_SYSTEM_H
