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

    private:
        std::chrono::time_point<std::chrono::steady_clock> last_time_updated_fps;
        float fps = 0;
        double elapsed = 0;
        double delta = 0;
    };
}

#endif //FLINT_ENGINE_H
