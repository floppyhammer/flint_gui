#include "core_server.h"

#include <sstream>

#include "../common/logger.h"

namespace Flint {

CoreServer::CoreServer() {
    last_time_updated_fps = std::chrono::high_resolution_clock::now();
}

void CoreServer::tick() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto current_time = std::chrono::high_resolution_clock::now();

    auto new_elapsed = std::chrono::duration<double, std::chrono::seconds::period>(current_time - start_time).count();

    delta = new_elapsed - elapsed;

    elapsed = new_elapsed;

    // Print FPS.
    std::chrono::duration<double> duration = current_time - last_time_updated_fps;
    if (duration.count() > 1) {
        last_time_updated_fps = current_time;

        fps = (float)(1.0 / delta);

        // Set frame time.
        std::ostringstream string_stream;
        string_stream << "FPS " << round(fps * 10.f) * 0.1f;
        Logger::verbose(string_stream.str(), "CoreServer");
    }
}

double CoreServer::get_delta() const {
    return delta;
}

double CoreServer::get_elapsed() const {
    return elapsed;
}

float CoreServer::get_fps() const {
    return fps;
}

} // namespace Flint
