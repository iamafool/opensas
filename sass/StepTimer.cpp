#include "StepTimer.h"
#include <stdexcept>

namespace sass {

    StepTimer::StepTimer()
        : running(false),
        startCpu(0),
        endCpu(0)
    {
        // Optionally, you can immediately start the timer
        start();
    }

    void StepTimer::start() {
        running = true;
        // Store the current time points
        startReal = std::chrono::steady_clock::now();
        startCpu = std::clock();
    }

    void StepTimer::stop() {
        if (!running) {
            throw std::runtime_error("Timer is not running, cannot stop");
        }
        running = false;

        endReal = std::chrono::steady_clock::now();
        endCpu = std::clock();
    }

    double StepTimer::getRealTimeSeconds() const {
        using namespace std::chrono;
        if (running) {
            // If user asks for elapsed time without stopping,
            // measure the difference now.
            auto now = steady_clock::now();
            auto realSec = duration_cast<duration<double>>(now - startReal).count();
            return realSec;
        }
        else {
            auto realSec = duration_cast<duration<double>>(endReal - startReal).count();
            return realSec;
        }
    }

    double StepTimer::getCpuTimeSeconds() const {
        // std::clock() returns the number of CPU clock ticks
        // used by the process. We can convert to seconds by dividing by CLOCKS_PER_SEC
        if (running) {
            std::clock_t now = std::clock();
            return static_cast<double>(now - startCpu) / CLOCKS_PER_SEC;
        }
        else {
            return static_cast<double>(endCpu - startCpu) / CLOCKS_PER_SEC;
        }
    }

} // namespace sass

