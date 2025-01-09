#include "timer.h"

#include <string>

#include "sub_window.h"

namespace Flint {

void Timer::start_timer(float time) {
    if (time <= 0) {
        return;
    }
    remaining_time_ = time;
    is_timer_stopped = false;
}

void Timer::update(double dt) {
    Node::update(dt);

    if (is_timer_stopped && remaining_time_ > 0) {
        remaining_time_ -= dt;
    }

    if (!is_timer_stopped && remaining_time_ <= 0) {
        is_timer_stopped = true;
        remaining_time_ = 0;
    }
}

void Timer::connect_signal(const std::string& signal, const AnyCallable<void>& callback) {
    Node::connect_signal(signal, callback);

    if (signal == "timeout") {
        timeout_callbacks.emplace_back(callback);
    }
}

void Timer::emit_timeout() {
    for (auto c : timeout_callbacks) {
        c();
    }
}

} // namespace Flint
