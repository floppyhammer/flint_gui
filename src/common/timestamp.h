#ifndef FLINT_TIMESTAMP_H
#define FLINT_TIMESTAMP_H

#include <vector>
#include <string>
#include <chrono>

namespace Flint {
    class Timestamp {
    private:
        std::chrono::time_point<std::chrono::steady_clock> start_time;

        std::vector<double> records; // In ms.
        std::vector<std::string> labels;

        bool enabled = true;

    public:
        Timestamp();

        void record(const std::string &p_label);

        void reset();

        void print();

        void set_enabled(bool p_enabled);
    };
}

#endif //FLINT_TIMESTAMP_H
