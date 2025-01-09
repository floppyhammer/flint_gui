#ifndef FLINT_TIMER_H
#define FLINT_TIMER_H

#include <memory>
#include <vector>

#include "../common/any_callable.h"
#include "../common/utils.h"
#include "../servers/engine.h"
#include "node.h"

namespace Flint {

class Timer final : public Node {
public:
    void start_timer(float time);

    void update(double dt) override;

    void connect_signal(const std::string& signal, const AnyCallable<void>& callback) override;

    float get_remaing_time() const;

protected:
    void emit_timeout();

    bool is_timer_stopped = true;
    double remaining_time_ = 0;
    std::vector<AnyCallable<void>> timeout_callbacks;
};

} // namespace Flint

#endif // FLINT_TIMER_H
