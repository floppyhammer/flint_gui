#ifndef FLINT_ECS_DATA_H
#define FLINT_ECS_DATA_H

//#ifndef FLINT_UUID
//    #define FLINT_UUID
//    #define UUID_SYSTEM_GENERATOR
//    #include <uuid.h>
//#endif

#include <optional>

namespace Flint::Ecs {

struct HandleId {
    enum class Type {
        Id,
        AssetPathId,
    } type;
    /// A handle id of a loaded asset.
    int uuid;

    /// A handle id of a pending asset.
    std::optional<std::string> asset_path_id;
};

template <typename T>
struct C_Handle {
    HandleId id;

    std::shared_ptr<T> data;
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_DATA_H
