#include "engine.h"

namespace Flint {
    Engine::Engine() = default;

    void Engine::tick() {
        static auto start_time = std::chrono::high_resolution_clock::now();
        auto current_time = std::chrono::high_resolution_clock::now();

        auto new_elapsed = std::chrono::duration<double, std::chrono::seconds::period>(current_time - start_time).count();

        delta = new_elapsed - elapsed;

        elapsed = new_elapsed;
    }

    double Engine::get_delta() const {
        return delta;
    }

    double Engine::get_elapsed() const {
        return elapsed;
    }
}
