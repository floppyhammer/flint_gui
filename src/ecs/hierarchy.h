#ifndef FLINT_ECS_HIERARCHY_H
#define FLINT_ECS_HIERARCHY_H

#include <entt/entt.hpp>

namespace Flint::Ecs {

struct C_Relations {
    std::size_t children{};
    entt::entity first_child{entt::null};
    entt::entity prev_sibling{entt::null};
    entt::entity next_sibling{entt::null};
    entt::entity parent{entt::null};
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_HIERARCHY_H
