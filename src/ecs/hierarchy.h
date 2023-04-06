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

std::vector<entt::entity> extract_entities_from_hierarchy(entt::registry &registry, entt::entity root);

} // namespace Flint::Ecs

#endif // FLINT_ECS_HIERARCHY_H
