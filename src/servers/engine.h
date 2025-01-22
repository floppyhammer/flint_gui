#ifndef FLINT_ENGINE_H
#define FLINT_ENGINE_H

#include <chrono>

namespace Flint {

enum class Signal {
    SwapChainChanged,
    RebuildCommandBuffer,
};

class Engine {
public:
    static Engine *get_singleton() {
        static Engine singleton;

        return &singleton;
    }

    Engine();

    void tick();

    double get_delta() const;

    double get_elapsed() const;

    float get_fps() const;

    int get_fps_int() const;

private:
#if defined(_WIN32) || defined(__APPLE__)
    std::chrono::time_point<std::chrono::steady_clock> last_time_updated_fps;
#elif __linux__
    std::chrono::time_point<std::chrono::system_clock> last_time_updated_fps;
#endif
    float fps = 0;
    double elapsed = 0;
    double delta = 0;
};

} // namespace Flint

#endif // FLINT_ENGINE_H
