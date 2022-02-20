#ifndef FLINT_ENGINE_H
#define FLINT_ENGINE_H

#include <chrono>

namespace Flint {
    class Engine {
    public:
        static Engine &getSingleton() {
            static Engine singleton;

            return singleton;
        }

        Engine();

        double elapsed = 0;

        void tick();
    };
}

#endif //FLINT_ENGINE_H
