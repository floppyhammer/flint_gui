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

template <class T>
struct C_Handle {
    HandleId id;

    std::shared_ptr<T> data;
};



#define CHECK_RESULT_RETURN(res) \
    if (!res.is_ok()) {          \
        return {res.error()};    \
    }

template <typename C, typename E>
class Result {
public:
private:
    enum class Type {
        Ok,
        Error,
    } _type;

    C _content{};
    E _error{};

public:
    Result(C content) {
        _type = Type::Ok;
        _content = content;
    }

    Result(E error) {
        _type = Type::Error;
        _error = error;
    }

    bool is_ok() const {
        return _type == Type::Ok;
    }

    C unwrap() const {
        if (!is_ok()) {
            abort();
        }
        return _content;
    }

    E error() const {
        return _error;
    }
};

} // namespace Flint::Ecs

#endif // FLINT_ECS_DATA_H
