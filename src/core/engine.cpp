#include "engine.h"

namespace Flint {
    Engine::Engine() {

    }

    void Engine::tick() {
        static auto start_time = std::chrono::high_resolution_clock::now();
        auto current_time = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
    }
}
