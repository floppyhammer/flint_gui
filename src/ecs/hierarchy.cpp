#include "hierarchy.h"

#include <iostream>

namespace Flint::Ecs {

void traverse_children(entt::registry &registry, entt::entity entity) {
    auto relations = registry.get<C_Relations>(entity);

    auto current_child = relations.first_child;

    while (true) {
        if (current_child != entt::null) {
            entt::entity child_entity = current_child;
            auto &child_relations = registry.get<C_Relations>(child_entity);
            std::cout << (uint32_t)child_entity << "  ";
            traverse_children(registry, child_entity);
            current_child = child_relations.next_sibling;
        } else {
            std::cout << "| ";
            break;
        }
    }
}

void hierarchy_system(entt::registry &registry, entt::entity root) {
    traverse_children(registry, root);
}

} // namespace Flint::Ecs
