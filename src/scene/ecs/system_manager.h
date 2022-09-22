#ifndef FLINT_SYSTEM_MANAGER_H
#define FLINT_SYSTEM_MANAGER_H

#include <cassert>
#include <memory>
#include <unordered_map>

#include "component.h"
#include "entity.h"
#include "system.h"

namespace Flint {
class SystemManager {
public:
    template <typename T>
    std::shared_ptr<T> register_system() {
        const char *type_name = typeid(T).name();

        assert(systems.find(type_name) == systems.end() && "Registering system more than once.");

        // Create a pointer to the system and return it, so it can be used externally.
        auto system = std::make_shared<T>();
        systems.insert({type_name, system});
        return system;
    }

    template <typename T>
    void set_signature(Signature signature) {
        const char *type_name = typeid(T).name();

        assert(systems.find(type_name) != systems.end() && "System used before registered.");

        // Set the signature for this system
        signatures.insert({type_name, signature});
    }

    void entity_destroyed(Entity entity) {
        // Erase a destroyed entity from all system lists.
        // `entities` is a set so no check needed.
        for (auto const &pair : systems) {
            auto const &system = pair.second;

            system->entities.erase(entity);
        }
    }

    void entity_signature_changed(Entity entity, Signature entity_signature) {
        // Notify each system that an entity's signature changed.
        for (auto const &pair : systems) {
            auto const &type = pair.first;
            auto const &system = pair.second;
            auto const &system_signature = signatures[type];

            // Entity signature matches system signature - insert into set.
            if ((entity_signature & system_signature) == system_signature) {
                system->entities.insert(entity);
            } else { // Entity signature does not match system signature - erase from set.
                system->entities.erase(entity);
            }
        }
    }

private:
    // Map from system type string pointer to a signature.
    std::unordered_map<const char *, Signature> signatures{};

    // Map from system type string pointer to a system pointer.
    std::unordered_map<const char *, std::shared_ptr<System>> systems{};
};
} // namespace Flint

#endif // FLINT_SYSTEM_MANAGER_H
