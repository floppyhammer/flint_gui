#ifndef FLINT_GRAPH_UTILS_H
#define FLINT_GRAPH_UTILS_H

namespace Flint::Ecs {

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
    Result(const C& content) {
        _type = Type::Ok;
        _content = content;
    }

    Result(const E& error) {
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

#endif // FLINT_GRAPH_UTILS_H
