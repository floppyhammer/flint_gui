#ifndef HIERARCHY_SYSTEM_H
#define HIERARCHY_SYSTEM_H

#include "../coordinator.h"
#include "../entity.h"
#include "../system.h"

namespace Flint {
class HierarchySystem : public System {
public:
    void propagate(const std::weak_ptr<Coordinator> &p_coordinator, Entity root);
};
} // namespace Flint

#endif // HIERARCHY_SYSTEM_H
