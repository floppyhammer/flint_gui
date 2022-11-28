#ifndef FLINT_CORE_SERVER_H
#define FLINT_CORE_SERVER_H

#include <chrono>

namespace Flint {

enum class Signal {
    SwapChainChanged,
    RebuildCommandBuffer,
};

class CoreServer {
public:
    static CoreServer *get_singleton() {
        static CoreServer singleton;

        return &singleton;
    }

    CoreServer();

    void tick();

    double get_delta() const;

    double get_elapsed() const;

    float get_fps() const;

private:
    std::chrono::time_point<std::chrono::steady_clock> last_time_updated_fps;
    float fps = 0;
    double elapsed = 0;
    double delta = 0;
};

} // namespace Flint

#endif // FLINT_CORE_SERVER_H
